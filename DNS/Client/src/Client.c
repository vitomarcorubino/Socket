/*
 ============================================================================
 Name        : Client.c
 Authors     : Giovanni Priore, Vito Marco Rubino, Simone Signorile
 Description : Processo client
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

#include <string.h>

#define BUFFERSIZE 512 // Dimensione del buffer
#define PROTOPORT 48000

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

	// Variabili per memorizzare le due stringhe digitate dal client
	char stringaA[BUFFERSIZE];
	char stringaB[BUFFERSIZE];

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

	/* ACQUISIZIONE DEL NUMERO DI PORTA DEL PROCESSO SERVER A CUI CI SI VUOLE CONNETTERE (l'IP è impostato a 127.0.0.1) */
	// Con l'indirizzo IP si identifica la macchina nella rete e con il numero di porta si identifica il processo su quella macchina.

	/* 2) IL CLIENT LEGGE DA TASTIERA IL NUMERO DI PORTA DA UTILIZZARE PER CONTATTARE IL SERVER */

	/*
	int numeroPorta;
	printf ("Numero di porta del server: ");
	scanf("%d", &numeroPorta);
	*/

	char * nomeHost;

	char nome[50];

	printf("Nome dell'host: ");
	scanf("%s", nome);

	nomeHost = &nome;

	struct hostent *host;

	host = gethostbyname(nomeHost);

	if (host == NULL) {
		printf("gethostbyname() failed.\n");
		exit(EXIT_FAILURE);
	} else {
		struct in_addr* ina = (struct in_addr*) host -> h_addr_list[0];
		printf("Risultato di gethostbyname(%s): %s\n", nomeHost, inet_ntoa(*ina));
	}

	/* COSTRUZIONE DELL'INDIRIZZO DEL SERVER */
	/*
	struct sockaddr_in indirizzoServer;
	struct sockaddr_in indirizzoRicevuto;
	int dimensioneIndirizzoRicevuto;

	memset(&indirizzoServer, 0, sizeof(indirizzoServer));

	indirizzoServer.sin_family = AF_INET;
	indirizzoServer.sin_addr.s_addr = inet_addr("127.0.0.1"); // La funzione inet_addr converte l'indirizzo in notazione puntata in un numero a 32 bit
	indirizzoServer.sin_port = htons(numeroPorta); // htons converte un numero dal formato del computer locale a quello della rete secondo il modello Big Endian
	*/
	/* 3) IL CLIENT RICHIEDE LA CONNESSIONE AL SERVER */
	/*
	if (connect(socketClient,(struct sockaddr *)&indirizzoServer, sizeof(indirizzoServer)) < 0){
		ErrorHandler ("Errore durante la connessione al server.\n");
		system("PAUSE");
		closesocket(socketClient);
		ClearWinSock();
		return -1;
	}
	*/

	/* Ricezione da server */
	/*
	int bytesRcvd;
	int totalBytesRcvd = 0;
	char buf[BUFFERSIZE];
	*/
	/*
	printf("\nServer: ");
	if ((bytesRcvd = recv(socketClient, buf, BUFFERSIZE - 1,0)) <= 0){
		ErrorHandler("recv() fallita.");
		closesocket(socketClient);
		ClearWinSock();
		return -1;
	}


	totalBytesRcvd = totalBytesRcvd + bytesRcvd;	// Numero byte ricevuti
	buf[bytesRcvd] = '\0';	// Terminatore di stringa utilizzato per far riconoscere la fine di una stringa alla printf()
	printf ("%s\n", buf);	// Stampa del buffer
	 */

	//char rispostaServer[BUFFERSIZE];

	//do {

		// memset(&rispostaServer, 0, sizeof(rispostaServer));

		/* 6) IL CLIENT LEGGE DALLO std input DUE STRINGHE A E B E LE INVIA AL SERVER */
		// Acquisizione dallo std input della prima stringa A

	/*
		printf("\nDigita la prima stringa (A)\n");
		scanf("\n");
		gets(stringaA);
	*/
		/* Invio della stringa A al server */
	/*
		if (sendto(socketClient, stringaA, strlen(stringaA), 0, (struct sockaddr*)&indirizzoServer, sizeof(indirizzoServer)) != strlen(stringaA)){
			ErrorHandler("Errore nell'invio messaggio");
			closesocket(socketClient);
			ClearWinSock();
			return -1;
		}

		int bytestring1;
	 */
		// Acquisizione dallo std input della seconda stringa B

	/*
		printf("Digita la seconda stringa (B)\n");
		scanf("\n");
		gets(stringaB);

		// Invio della seconda stringa B al server
		if (sendto(socketClient, stringaB, strlen(stringaB), 0, (struct sockaddr*)&indirizzoServer, sizeof(indirizzoServer)) != strlen(stringaB)){
			ErrorHandler("Errore nell'invio messaggio");
			closesocket(socketClient);
			ClearWinSock();
			return -1;
		}
*/

		/**
		 * 8)IL CLIENT RICEVE LA STRINGA INVIATA DAL SERVER E LA STAMPA A VIDEO.
		 *   SE LA STRINGA VISUALIZZATA È UGUALE A "bye", CHIUDE LA CONNESSIONE E
		 *   IL PROCESSO TERMINA (dopo il do-while);
		 *   ALTRIMENTI IL CLIENT TORNA AL PASSO 6
		 */
/*
		dimensioneIndirizzoRicevuto = sizeof(indirizzoRicevuto);
		bytestring1 = recvfrom(socketClient, rispostaServer, BUFFERSIZE - 1, 0, (struct sockaddr*)&indirizzoRicevuto, &dimensioneIndirizzoRicevuto);

		if (indirizzoServer.sin_addr.s_addr != indirizzoRicevuto.sin_addr.s_addr) {
			printf("Errore: pacchetto ricevuto da una sorgente sconosciuta.\n");
			exit(EXIT_FAILURE);
		}

		rispostaServer[bytestring1] = '\0';
		printf("Server: %s\n", rispostaServer);
*/
	//} while (strcmp(rispostaServer, "bye") != 0); // Il ciclo si ripete finchè il server non invia al client la stringa "bye"

	// Chiusura della socket del client
	closesocket(socketClient);
	ClearWinSock();
	printf ("\n");
	system("PAUSE");
	return 0;

}
