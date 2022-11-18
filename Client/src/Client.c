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
#endif

/**
 * @brief Libreria per la gestione di funzioni di input e output
 */
#include <stdio.h>

#define BUFFERSIZE 512
#define PROTOPROT 27015
// #define DIMENSIONEINDIRIZZO 15 // La dimensione di un indirizzo IP in notazione puntata è di 15 caratteri [xxx.xxx.xxx.xxx]

void ErrorHandler (char *errorMessage){
	printf ("%s", errorMessage);
}

void ClearWinSock() {
	#if defined WIN32
		WSACleanup();
	#endif
}

int main(void){
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
	socketClient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	/**
	 * @brief se la creazione della socket fallisce, termina il programma.
	 */
	if (socketClient < 0){
		ErrorHandler ("Creazione della socket fallita.\n");
		closesocket(socketClient);
		ClearWinSock();
		return -1;
	}

	/* ACQUISIZIONE DEI DATI DEL SERVER A CUI CI SI VUOLE CONNETTERE [Indirizzo IP + numero di porta del processo] */
	// Con l'indirizzo IP si identifica la macchina nella rete e con il numero di porta si identifica il processo su quella macchina.

	/*
	char indirizzo[DIMENSIONEINDIRIZZO] = "\0";
	printf ("Indirizzo del server: ");
	scanf("\n");
	gets(indirizzo);
	*/

	/* 2) IL CLIENT LEGGE DA TASTIERA IL NUMERO DI PORTA DA UTILIZZARE PER CONTATTARE IL SERVER */
	int numeroPorta;
	printf ("Numero di porta del server: ");
	scanf("%d", &numeroPorta);

	/* COSTRUZIONE DELL'INDIRIZZO DEL SERVER */
	struct sockaddr_in indirizzoServer;

	memset(&indirizzoServer, 0, sizeof(indirizzoServer));

	indirizzoServer.sin_family = AF_INET;
	indirizzoServer.sin_addr.s_addr = inet_addr("127.0.0.1"); // La funzione inet_addr converte l'indirizzo in notazione puntata in un numero a 32 bit
	indirizzoServer.sin_port = htons(numeroPorta); // htons converte un numero dal formato del computer locale a quello della rete secondo il modello Big Endian

	/* 3) IL CLIENT RICHIEDE LA CONNESSIONE AL SERVER */
	if (connect(socketClient,(struct sockaddr *)&indirizzoServer, sizeof(indirizzoServer)) < 0){
		ErrorHandler ("Errore durante la connessione al server.\n");
		system("PAUSE");
		closesocket(socketClient);
		ClearWinSock();
		return -1;
	}

	/* Ricezione da server */
	int bytesRcvd;
	int totalBytesRcvd = 0;
	char buf[BUFFERSIZE];

	printf ("Server: ");
	if ((bytesRcvd = recv(socketClient, buf, BUFFERSIZE - 1,0)) <= 0){
		ErrorHandler("recv() fallita.");
		closesocket(socketClient);
		ClearWinSock();
		return -1;
	}

	totalBytesRcvd = totalBytesRcvd + bytesRcvd;	// Numero byte ricevuti
	buf[bytesRcvd] = '\0';	// Terminatore di stringa utilizzato per far riconoscere la fine di una stringa alla printf()
	printf ("%s\n", buf);	// Stampa del buffer

	/* INIZIO PARTE LOGICA */
	char acknowledgement[BUFFERSIZE];
	do {
		memset(&acknowledgement, 0, sizeof(acknowledgement));

		/* 6) IL CLIENT LEGGE DALLO std input DUE STRINGHE A E B E LE INVIA AL SERVER */
		printf("Digita la prima stringa (A)\n");
		scanf("%s", stringaA);

		/* Inviare welcome a server */
		if (send(socketClient, stringaA, strlen(stringaA), 0) != strlen(stringaA)){
			ErrorHandler("Errore nell'invio messaggio");
			closesocket(socketClient);
			ClearWinSock();
			return -1;
		}

		/* Ricezione stringa dal server */
		int bytestring1;

		bytestring1=recv(socketClient, acknowledgement, BUFFERSIZE - 1, 0);
		acknowledgement[bytestring1]='\0';
		printf ("Server: %s\n", acknowledgement);

		printf("Digita la seconda stringa (B)\n");
		scanf ("%s", stringaB);


		if (send(socketClient, stringaB,strlen(stringaB),0) != strlen(stringaB)){
			ErrorHandler("Errore nell'invio messaggio");
			closesocket(socketClient);
			ClearWinSock();
			return -1;
		}

		/**
		 * 8)IL CLIENT RICEVE LA STRINGA INVIATA DAL SERVER E LA STAMPA A VIDEO.
		 *   SE LA STRINGA VISUALIZZATA È UGUALE A "bye", CHIUDE LA CONNESSIONE E
		 *   IL PROCESSO TERMINA (dopo il do-while);
		 *   ALTRIMENTI IL CLIENT TORNA AL PASSO 6
		 */
		bytestring1 = recv(socketClient, acknowledgement, BUFFERSIZE - 1, 0);
		acknowledgement[bytestring1] = '\0';
		printf("Server: %s\n", acknowledgement);


		bytestring1 = recv(socketClient, acknowledgement, BUFFERSIZE - 1, 0);
		acknowledgement[bytestring1] = '\0';
		printf("Server: %s\n", acknowledgement);

	} while (strcmp(acknowledgement, "bye") != 0);

	// Chiusura della socket del client
	closesocket(socketClient);
	ClearWinSock();
	printf ("\n");
	system("PAUSE");
	return 0;

}