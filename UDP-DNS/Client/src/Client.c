/*
 ============================================================================
 Name        : Client.c
 Author      : Priore Giovanni, Rubino Vito Marco, Signorile Simone
 Version     :
 Description : Processo client per comunicazione UDP - DNS
 ============================================================================
 */

/**
 * @brief Codice per implementare un programma portabile, cioè adatto sia a piattaforma Windows che Unix
 */
#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif

/**
 * @brief Libreria per la gestione di funzioni di input e output
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define BUFFERSIZE 512 // Dimensione del buffer

bool vocale(char carattere); // Prototipo di funzione che verifica se un carattere è una vocale

void ErrorHandler (char *errorMessage){
	printf ("%s", errorMessage);
}

void ClearWinSock() {
	#if defined WIN32
		WSACleanup();
	#endif
}

int main(){
	#if defined WIN32
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD (2,2), &wsaData);
		if (iResult != 0){
			printf ("Errore in WSASturtup\n");
			return -1;
		}
	#endif

	/* Creazione della socket client */
	int socketClient;
					// Si utilizza SOCK_DGRAM perchè UDP supporta i datagrammi
	socketClient = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	/**
	 * @brief se la creazione della socket fallisce, termina il programma.
	 */
	if (socketClient < 0){
		ErrorHandler ("Creazione della socket fallita.\n");
		closesocket(socketClient);
		ClearWinSock();
		return -1;
	}

	/*
	 * 1) IL CLIENT LEGGE DA TASTIERA IL NOME DELL'HOST E IL NUMERO DI PORTA DA UTILIZZARE PER CONTATTARE IL SERVER
	 *    Con il nome dell'host, tramite la risoluzione dei nomi con DNS, sarà possibile ricavare l'indirizzo IP del server.
	 *    Con l'indirizzo IP si identifica la macchina nella rete e con il numero di porta si identifica il processo su quella macchina.
	 */
	char nomeHost[50]; // Nome dell'host acquisito da tastiera
	char * ptrNomeHost; // Puntatore al nome dell'host acquisito da tastiera

	// Acquisizione del nome dell'host
	printf("Nome dell'host: ");
	scanf("%s", nomeHost);

	// Assegnazione dell'indirizzo di memoria in cui si trova il nome dell'host al relativo puntatore
	ptrNomeHost = &nomeHost;

	// Struttura contenente tutte le informazioni sull'host, tra cui il suo nome simbolico, alias e lista di indirizzi.
	struct hostent *host;

	// La seguente funzione effettua la risoluzione del nome simbolico a indirizzo Internet.
	host = gethostbyname(ptrNomeHost);

	// Se la funzione di risoluzione del nome simbolico restituisce null, si è verificato un errore. Quindi, il programma termina.
	if (host == NULL) {
		printf("gethostbyname() failed.\n");
		exit(EXIT_FAILURE);
	} else {
		// Questa assegnazione consente di pescare l'indirizzo ip corrispondente al nome dell'host da dentro la struttura hostent.
		struct in_addr* ina = (struct in_addr*) host -> h_addr_list[0];
		printf("Risultato di gethostbyname(%s): %s\n", ptrNomeHost, inet_ntoa(*ina));
	}

	// Una volta ricavato l'indirizzo IP associato al server, si acquisisce il numero di porta che identifica il suo processo.
	int numeroPorta;
	printf ("Numero di porta del server: ");
	scanf("%d", &numeroPorta);

	/* Costruzione dell'indirizzo del server */
	struct sockaddr_in indirizzoServer; // Indirizzo del server che il client desidera contattare
	 // Indirizzo della risorsa che ha risposto al client, da confrontare con l'indirizzo del server contattato.
	// Se l'indirizzo ricevuto non corrisponde all'indirizzo del server contattato, significa che il messaggio non proviene dall'host desiderato.
	struct sockaddr_in indirizzoRicevuto;
	int dimensioneIndirizzoRicevuto;

	memset(&indirizzoServer, 0, sizeof(indirizzoServer));
	dimensioneIndirizzoRicevuto = sizeof(indirizzoRicevuto);

	indirizzoServer.sin_family = AF_INET;
	indirizzoServer.sin_addr.s_addr = inet_addr("127.0.0.1"); // La funzione inet_addr converte l'indirizzo in notazione puntata in un numero a 32 bit
	indirizzoServer.sin_port = htons(numeroPorta); // htons converte un numero dal formato del computer locale a quello della rete secondo il modello Big Endian

	/* 2) IL CLIENT CONTATTA IL SERVER INVIANDO UN MESSAGGIO INIZIALE A PIACERE */
	char messaggioIniziale[BUFFERSIZE];
	printf("\nDigita un messaggio a piacere: ");
	scanf("\n");
	gets(messaggioIniziale);

	if (sendto(socketClient, messaggioIniziale, strlen(messaggioIniziale), 0, (struct sockaddr*)&indirizzoServer, sizeof(indirizzoServer)) != strlen(messaggioIniziale)){
		ErrorHandler("Errore nell'invio messaggio");
		closesocket(socketClient);
		ClearWinSock();
		return -1;
	}

	// Vettore di caratteri (stringa) in cui si memorizza la risposta del server
	char rispostaServer[BUFFERSIZE];

	// Azzeramento dello spazio di memoria dedicato alla risposta del server
	memset(&rispostaServer, 0, sizeof(rispostaServer));

	/* IL CLIENT RICEVE LA STRINGA "OK" E LA VISUALIZZA SULLO STD OUTPUT*/
	int bytestring1;
	bytestring1 = recvfrom(socketClient, rispostaServer, BUFFERSIZE - 1, 0, (struct sockaddr*)&indirizzoRicevuto, &dimensioneIndirizzoRicevuto);

	// Controllo per verificare la corrispondenza tra indirizzo contattato e indirizzo che ha risposto.
	if (indirizzoServer.sin_addr.s_addr != indirizzoRicevuto.sin_addr.s_addr) {
		printf("Errore: pacchetto ricevuto da una sorgente sconosciuta.\n");
		exit(EXIT_FAILURE);
	}

	rispostaServer[bytestring1] = '\0';
	printf("Server: %s\n", rispostaServer);


	char stringa[BUFFERSIZE];
	int i, lunghezzaStringa;
	char *carattereVocale;
	int numeroVocali = 0;

	/*
	 * 6) IL CLIENT LEGGE UNA STRINGA DI CARATTERI ALFANUMERICI DALLO STD INPUT E CONTA IL NUMERO DI VOCALI
	 *    PRESENTI NELLA STRINGA.
	 *    A QUESTO PUNTO, IL CLIENT INVIA AL SERVER OGNI VOCALE TROVATA MEDIANTE UNA sendto SEPARATA.
	 */

	// Acquisizione della stringa che sarà inviata al server per far trasformare le vocali in maiuscolo
	printf("\nDigita una stringa: ");
	gets(stringa);
	printf("\n");

	// Salviamo la lunghezza della stringa che ci servirà  nei vari confronti
	lunghezzaStringa = strlen(stringa);

	// Conto il numero di vocali presenti nella stringa digitata
	for (i = 0; i < lunghezzaStringa; i++) {
		if (vocale(stringa[i])) {
			numeroVocali++;
		}
	}

	// Trasformo in stringa il contatore delle vocali per poterlo inviare come messaggio al server
	char strVocali[sizeof(int)*8+1];
	itoa(numeroVocali, strVocali, 10);

	// Invio al server il numero di vocali come stringa, in modo che possa determinare il numero di receive necessaria a trasformare le vocali in maiuscolo
	if (sendto(socketClient, strVocali, strlen(strVocali), 0, (struct sockaddr*)&indirizzoServer, sizeof(indirizzoServer)) != strlen(strVocali)){
		ErrorHandler("Errore nell'invio messaggio");
		closesocket(socketClient);
		ClearWinSock();
		return -1;
	}


	for (i = 0; i < lunghezzaStringa; i++){
		if(vocale(stringa[i])){
			carattereVocale = stringa[i];

			// Invio al server ogni vocale trovata
			if (sendto(socketClient, &carattereVocale, strlen(&carattereVocale), 0, (struct sockaddr*)&indirizzoServer, sizeof(indirizzoServer)) != strlen(&carattereVocale)){
				ErrorHandler("Errore nell'invio messaggio");
				closesocket(socketClient);
				ClearWinSock();
				return -1;
			}

			/* 8) IL CLIENT VISUALIZZA SULLO STD OUTPUT OGNI VOCALE MAIUSCOLA INVIATA DAL SERVER */
			// Ricevo dal server ogni vocale inviata, trasformata in maiuscolo
			dimensioneIndirizzoRicevuto = sizeof(indirizzoRicevuto);
			bytestring1 = recvfrom(socketClient, rispostaServer, BUFFERSIZE - 1, 0, (struct sockaddr*)&indirizzoRicevuto, &dimensioneIndirizzoRicevuto);

			if (indirizzoServer.sin_addr.s_addr != indirizzoRicevuto.sin_addr.s_addr) {
					printf("Errore: pacchetto ricevuto da una sorgente sconosciuta.\n");
					exit(EXIT_FAILURE);
			}

			rispostaServer[bytestring1] = '\0';
			printf("Server: %c\n", rispostaServer[0]);

		}
	}

	/* 9) IL CLIENT TERMINA */
	// Chiusura della socket del client
	closesocket(socketClient);
	ClearWinSock();
	printf ("\n");
	system("PAUSE");

	return 0;
}

// Funzione per verificare se un carattere passato come parametro è una vocale.
// Restituisce true se il carattere è una vocale, false altrimenti.
bool vocale(char carattere) {
	bool vocale;

	vocale = false;

	if ((carattere == 'a' || carattere == 'e' || carattere == 'i' || carattere == 'o' || carattere == 'u')) {
		// || (carattere == 'A' || carattere == 'E' || carattere == 'I' || carattere == 'O' || carattere == 'U')
		vocale = true;
	}

	return vocale;
}
