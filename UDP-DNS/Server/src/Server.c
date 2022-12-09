/*
 ============================================================================
 Name        : Server.c
 Authors     : Giovanni Priore, Vito Marco Rubino, Simone Signorile
 Description : Processo Server per comunicazione UDP - DNS
 ============================================================================
 */

/**
 * @brief Librerie per la gestione di funzioni di input/output
 */
#include <stdio.h>
#include <stdlib.h>
/**
 * @brief Librerie per la gestione di stringhe
 */
#include <string.h>
#include <ctype.h>

/**
 * @brief Codice per implementare un programma portabile, cioè adatto sia a piattaforma Windows che Unix
 */
#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define BUFFERSIZE 512 // Dimensione del buffer
#define PROTOPORT 48000	// Numero di porta su cui è attivo il processo server

void ErrorHandler (char *errorMessage){
	printf ("%s",errorMessage);
}

void ClearWinSock (){
#if defined WIN32
	WSACleanup();
#endif
}

int main(){
	#if defined WIN32
		WSADATA WSAData;
		int iresult = WSAStartup(MAKEWORD(2,2), &WSAData);
		if (iresult!=0){
			ErrorHandler("Errore in WSAStartup\n");
			return -1;
		}
	#endif

	/* Socket utilizzata dal server */
	int socketServer;

	/* Si chiama la funzione socket() assegnando il valore di ritorno alla variabile appena creata */
					   // Si utilizza SOCK_DGRAM perchè UDP supporta i datagrammi
	socketServer = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	/* Se la creazione della socket genera errori, il programma termina */
	if (socketServer < 0){
		ErrorHandler("Creazione della socket fallita.\n");
		closesocket(socketServer);
		ClearWinSock();
		return -1;
	}

	/* COSTRUZIONE DELL'INDIRIZZO DEL SERVER */
	// Si costruisce lato server per fare successivamente la bind()
	struct sockaddr_in indirizzoServer;

	memset(&indirizzoServer, 0, sizeof(indirizzoServer));
	// Si avvalora indirizzoServer assegnando un indirizzo localhost
	indirizzoServer.sin_family = AF_INET;
	indirizzoServer.sin_addr.s_addr = inet_addr("127.0.0.1"); // La funzione inet_addr converte l'indirizzo in notazione puntata in un numero a 32 bit
	// htons converte un numero dal formato del computer locale a quello della rete secondo il modello Big Endian
	indirizzoServer.sin_port = htons(PROTOPORT); // 48000 è il numero di porta di default definito all'inizio

	/* Il server è avviato su una data porta [48000] */
	/**
	* @brief bind() associa alla socket un indirizzo in modo da poter essere contatta da un client
	* e in caso di errore termina l'esecuzione del programma
	*/
	if (bind(socketServer, (struct sockaddr*)&indirizzoServer, sizeof(indirizzoServer)) < 0){
		ErrorHandler("Operazione di bind fallita.\n");
		closesocket(socketServer);
		ClearWinSock();
		return 0;
	}

	// Costruzione dell'indirizzo del client
	struct hostent *host;
	struct in_addr nomeHost;
	struct sockaddr_in indirizzoClient;
	int lunghezzaClient;

	// Primo messaggio a piacere che il server riceverà dal client
	char stringaIniziale[BUFFERSIZE];
	int bytestringIniziale;

	// Il ciclo è infinito perchè il Server resta sempre in ascolto
	while (1) {
		lunghezzaClient = sizeof(indirizzoClient);

		// Il server ricevere il primo messaggio a piacere dal client
		bytestringIniziale = recvfrom(socketServer, stringaIniziale, BUFFERSIZE - 1, 0, (struct sockaddr*)&indirizzoClient, &lunghezzaClient);
		stringaIniziale[bytestringIniziale] = '\0';

		// Si effettua il passaggio da indirizzo Internet a nome simbolico
		nomeHost.s_addr = inet_addr(inet_ntoa(indirizzoClient.sin_addr));
		host = gethostbyaddr((char *) &nomeHost, 4, AF_INET);
		char* nomeSimbolico = host->h_name; // Si pesca dalla struttura hostent il nome simbolico associato all'indirizzo IP del client che ha contattato il server

		/* 3) IL SERVER VISUALIZZA SULLO STD OUTPUT IL MESSAGGIO RICEVUTO INSIEME AL NOME DELL'HOST DEL CLIENT
		 *    CHE L'HA APPENA CONTATTATO.
		 */
		printf("\"%s\" ricevuto dal client con nome host: %s\n", stringaIniziale, nomeSimbolico);

		/* 4) IL SERVER INVIA AL CLIENT LA STRINGA "OK" */
		char stringaOK[3] = "OK";
		stringaOK[3] = '\0';
		if (sendto(socketServer, stringaOK, strlen(stringaOK), 0, (struct sockaddr*)&indirizzoClient, sizeof(indirizzoClient)) != strlen(stringaOK)){
				ErrorHandler("Errore nell'invio messaggio");
				closesocket(socketServer);
				ClearWinSock();
				return -1;
		}


		int bytestringNumeroVocali;
		char strNumeroVocali[BUFFERSIZE];
		// Si riceve dal client il numero di vocali sotto forma di stringa
		bytestringNumeroVocali = recvfrom(socketServer, strNumeroVocali, BUFFERSIZE - 1, 0, (struct sockaddr*)&indirizzoClient, &lunghezzaClient);
		strNumeroVocali[bytestringNumeroVocali] = '\0';
		// Si converte il numero di vocali da stringa a intero
		int numeroVocali = atoi(strNumeroVocali);

		// Si ricevono dal client numeroVocali vocali e le si inviano trasformate in maiuscolo
		char stringaVocale[BUFFERSIZE];
		int bytestringVocale;
		int i;
		for (i = 0; i < numeroVocali; i++) {
			/* 7) IL SERVER PER OGNI VOCALE RICEVUTA INVIA, ALLO STESSO CLIENT DAL QUALE L'HA APPENA RICEVUTA, LA STESSA VOCALE MA IN MAIUSCOLO */
			bytestringVocale = recvfrom(socketServer, stringaVocale, BUFFERSIZE - 1, 0, (struct sockaddr*)&indirizzoClient, &lunghezzaClient);
			stringaVocale[bytestringVocale] = '\0';
			printf("\"%s\" ricevuto dal client con nome host: %s\n", stringaVocale, nomeSimbolico);

			char vocaleMaiuscola[BUFFERSIZE];
			vocaleMaiuscola[0] = toupper(stringaVocale[0]);
			if (sendto(socketServer, &vocaleMaiuscola[0], strlen(&vocaleMaiuscola[0]), 0, (struct sockaddr*)&indirizzoClient, sizeof(indirizzoClient)) != strlen(&vocaleMaiuscola[0])) {
					ErrorHandler("Errore nell'invio messaggio");
					closesocket(socketServer);
					ClearWinSock();
					return -1;
			}
		}
		printf("\n");
	}
}
