/*
 ============================================================================
 Name        : Server.c
 Authors     : Giovanni Priore, Vito Marco Rubino, Simone Signorile
 Version     :
 Copyright   :
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
 *
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
#define PROTOPORT 27015	// Numero di porta di default del protocollo
#define QLEN 6 // Massimo numero di connessioni che può accettare la socket tramite listen()

void ErrorHandler (char *errorMessage){
	printf ("%s",errorMessage);
}

void ClearWinSock (){
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]){

	int port;
	if (argc > 1){
		port = atoi(argv[1]);
	}
	else port = PROTOPORT;	// Default
	if (port < 0){
		printf ("Numero di porta non valido %s \n",argv[1]);
		return -1;
	}

#if defined WIN32
	WSADATA WSAData;
	int iresult = WSAStartup(MAKEWORD(2,2), &WSAData);
	if (iresult!=0){
		ErrorHandler("Errore in WSAStartup\n");
		return -1;
	}
#endif

	/* Socket utilizzata per instaurare una connessione */
	int NewSocket;

	/* Chiamare la funzione socket() assegnando il valore di ritorno alla variabile appena creata */
	NewSocket=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	/* 0 perchè indica il protocollo di default per la famiglia di protocolli e il tipo */

	/* Se la creazione della socket di benvenuto genera errori, il programma termina */
	if (NewSocket < 0){
		ErrorHandler("Creazione della socket fallita.\n");
		closesocket(NewSocket);
		ClearWinSock();
		return -1;
	}


	struct sockaddr_in sad;

	memset(&sad,0,sizeof(sad));
	// Si avvalora serverAddress assegnando un indirizzo localhost
	sad.sin_family=AF_INET;
	sad.sin_addr.s_addr=inet_addr("127.0.0.1"); // La funzione inet_addr converte l'indirizzo in notazione puntata in un numero a 32 bit
	// htons converte un numero dal formato del computer locale a quello della rete secondo il modello Big Endian
	sad.sin_port=htons(27015); // 27015 è il numero di porta di default definito all'inizio

	/**
	* @brief bind() associa alla socket un indirizzo in modo da poter essere contatta da un client
	* e in caso di errore termina l'esecuzione del programma
	*/
	if (bind(NewSocket,(struct sockaddr*)&sad,sizeof(sad))<0){
		ErrorHandler("Operazione di bind fallita.\n");
		closesocket(NewSocket);
		ClearWinSock();
		return 0;
	}

	// Si imposta la socket del server all'ascolto
	if (listen(NewSocket, QLEN)<0){
		ErrorHandler("Operazione di listen fallita.\n");
		closesocket(NewSocket);
		ClearWinSock();
		return -1;
	}

	// New Connection
	struct sockaddr_in cad;
	int clientSocket;
	int clientLen;
	int stringLen=0;
	printf ("In attesa del client...");
	while(1){
		clientLen=sizeof(cad);
		if ((clientSocket=accept(NewSocket,(struct sockaddr*)&cad,&clientLen))<0){
			ErrorHandler("Accettazione fallita.\n");
			closesocket(NewSocket);
			ClearWinSock();
			return-1;
		}
		printf ("Indirizzo client %s\n",inet_ntoa(cad.sin_addr));
		char*buf="Connessione instaurata";
		stringLen=strlen(buf);
		if (send(clientSocket,buf,stringLen,0)!=stringLen){
			ErrorHandler("E' stato inviato un numero differente di byte.");
			closesocket(clientSocket);
			ClearWinSock();
			system("PAUSE");
			return -1;
		}
		int bytestring1;
		char string1[BUFFERSIZE];
		bytestring1=recv(clientSocket,string1,BUFFERSIZE-1,0);
		string1[bytestring1]='\0';
		printf ("Client: %s\n",string1);
		char welcome[BUFFERSIZE]="ack";
		if(send(clientSocket,welcome,strlen(welcome),0)!=strlen(welcome)){
			ErrorHandler("E' stato inviato un numero differente di byte.");
			closesocket(clientSocket);
			ClearWinSock();
			system("PAUSE");
			return -1;
		}
	}
}
