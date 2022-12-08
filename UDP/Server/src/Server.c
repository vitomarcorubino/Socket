/*
 ============================================================================
 Name        : Server.c
 Authors     : Giovanni Priore, Vito Marco Rubino, Simone Signorile
 Description : Processo Server
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
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define BUFFERSIZE 512 // Dimensione del buffer
#define PROTOPORT 48000	// Numero di porta su cui è attivo il processo server
// #define QLEN 6 // Massimo numero di connessioni che può accettare la socket tramite listen()

void ErrorHandler (char *errorMessage){
	printf ("%s",errorMessage);
}

void ClearWinSock (){
#if defined WIN32
	WSACleanup();
#endif
}

// int main(int argc, char *argv[]){
int main(){
	/*
	int porta;
	if (argc > 1){
		porta = atoi(argv[1]);
	}
	else porta = PROTOPORT; //  Il server è attivo sulla porta 27015
	if (porta < 0){
		printf ("Numero di porta non valido %s \n", argv[1]);
		return -1;
	}
	*/
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

	/* 1) IL SERVER È AVVIATO SU UNA DATA PORTA [27015] */
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

	/*
	// Si imposta la socket del server all'ascolto
	if (listen(socketBenvenuto, QLEN) < 0){
		ErrorHandler("Operazione di listen fallita.\n");
		closesocket(socketBenvenuto);
		ClearWinSock();
		return -1;
	}
	*/


	// Costruzione dell'indirizzo del client
	struct sockaddr_in indirizzoClient;
	struct sockaddr_in indirizzoRicevuto;
	int dimensioneIndirizzoRicevuto;
	// int socketConnessione;
	int lunghezzaClient;
	//int stringLen = 0;
	//printf("In attesa del client...\n\n");

	// Il ciclo è infinito perchè il Server resta sempre in ascolto
	while(1){
		lunghezzaClient = sizeof(indirizzoClient);

		/*
		// socket connessione
		if ((socketConnessione = accept(socketBenvenuto,(struct sockaddr*)&indirizzoClient,&lunghezzaClient))<0){
			ErrorHandler("Accettazione fallita.\n");
			closesocket(socketBenvenuto);
			ClearWinSock();
			return-1;
		}
		*/

		// Inviamo al client la conferma di avvenuta connessione
		//char *confermaConnessione = "Connessione avvenuta";
		//stringLen = strlen(confermaConnessione);
		/* 5) IL SERVER INVIA AL CLIENT LA STRINGA "Connessione avvenuta" */
		/*
		if (send(socketConnessione, confermaConnessione, stringLen, 0) != stringLen){
			ErrorHandler("E' stato inviato un numero differente di byte.");
			closesocket(socketConnessione);
			ClearWinSock();
			system("PAUSE");
			return -1;
		}
		*/

		int bytestringA;
		char stringaA[BUFFERSIZE];
		int bytestringB;
		char stringaB[BUFFERSIZE];

	//	do {
			// Ricezione della prima stringa A
			bytestringA = recvfrom(socketServer, stringaA, BUFFERSIZE - 1, 0, (struct sockaddr*)&indirizzoClient, &lunghezzaClient);
			stringaA[bytestringA] = '\0';
			printf("Ricevuto dal client %s:\n", inet_ntoa(indirizzoClient.sin_addr));
			printf("%s\n", stringaA);

			/* 4) STABILITA LA CONNESSIONE, IL SERVER VISUALIZZA SULLO std output UN MESSAGGIO CONTENENTE
					 *    L'IP DEL CLIENT CON CUI HA STABILITO LA CONNESSIONE.
					 */

			// Ricezione della seconda stringa B



			dimensioneIndirizzoRicevuto = sizeof(indirizzoRicevuto);
			bytestringB = recvfrom(socketServer, stringaB, BUFFERSIZE - 1, 0, (struct sockaddr*)&indirizzoRicevuto, &lunghezzaClient);

			if (indirizzoServer.sin_addr.s_addr != indirizzoRicevuto.sin_addr.s_addr) {
				printf("Errore: pacchetto ricevuto da una sorgente sconosciuta.\n");
			//	exit(EXIT_FAILURE);
			} else {
				printf("Ricevuto dal client %s:\n", inet_ntoa(indirizzoClient.sin_addr));
				stringaB[bytestringB] = '\0';
				printf("%s\n\n", stringaB);

							/*
							 * 7) IL SERVER RICEVE LE STRINGHE A E B E LE CONCATENA IN UN UNICA STRINGA C UGUALE AD "A + B"
							 *    E LA INVIA DIETRO AL CLIENT; ALTRIMENTI, SE ALMENO UNA DELLE DUE STRINGHE A O B È UGUALE A "quit"
							 *    IL SERVER INVIA AL CLIENT LA STRINGA "bye"
							 */

				//if ((strcmp(stringaA, "quit") != 0) && (strcmp(stringaB, "quit") != 0)) {
				char stringaC[BUFFERSIZE] = "Stringa C [A + B] =  ";
				strcat(stringaC, stringaA);
				strcat(stringaC, stringaB);

								// Invio della stringa concatenata al client
				if(sendto(socketServer, stringaC, strlen(stringaC), 0, (struct sockaddr*)&indirizzoClient, sizeof(indirizzoClient)) != strlen(stringaC)) {
					ErrorHandler("E' stato inviato un numero differente di byte.");
					closesocket(socketServer);
					ClearWinSock();
					system("PAUSE");
					return -1;
				}
				//}
			}


			/*
			 * 7) IL SERVER RICEVE LE STRINGHE A E B E LE CONCATENA IN UN UNICA STRINGA C UGUALE AD "A + B"
			 *    E LA INVIA DIETRO AL CLIENT; ALTRIMENTI, SE ALMENO UNA DELLE DUE STRINGHE A O B È UGUALE A "quit"
			 *    IL SERVER INVIA AL CLIENT LA STRINGA "bye"
			 */
			/*
			if ((strcmp(stringaA, "quit") != 0) && (strcmp(stringaB, "quit") != 0)) {
				char stringaC[BUFFERSIZE] = "Stringa C [A + B] =  ";
				strcat(stringaC, stringaA);
				strcat(stringaC, stringaB);

				// Invio della stringa concatenata al client
				if(send(socketConnessione, stringaC, strlen(stringaC), 0) != strlen(stringaC)){
					ErrorHandler("E' stato inviato un numero differente di byte.");
					closesocket(socketConnessione);
					ClearWinSock();
					system("PAUSE");
					return -1;
				}
			}
			*/
			// La ricezione di stringhe si interrompe quando almeno una delle due è quit
		//} while ((strcmp(stringaA, "quit") != 0) && (strcmp(stringaB, "quit") != 0));
		/*
		char bye[5] = "bye";

		// Invio di "bye" al server e chiusura della socket
		if(send(socketConnessione, bye, strlen(bye), 0) != strlen(bye)){
			ErrorHandler("E' stato inviato un numero differente di byte.");
			closesocket(socketConnessione);
			ClearWinSock();
			system("PAUSE");
			return -1;
		}
		*/
	}
}
