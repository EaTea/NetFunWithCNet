#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // for memset()
#include <unistd.h>      // for close()
#include <sys/socket.h>  // for socket(), connect(), send(), and recv()
#include <sys/types.h>
#include <arpa/inet.h>   // for sockaddr_in and inet_addr()
#include <netinet/in.h>
#include "ipod3230.h"


static const int MAXPENDING = 5; //Max. outstanding connection requests

void DieWithUserMessage(const char *msg, const char *detail) {
    fputs(msg, stderr);
    fputs(": ",stderr);
    fputs(detail, stderr);
    fputc('\n', stderr);
    exit(1);
}

void DieWithSystemMessage(const char *msg) {
    perror(msg);
    exit(1);
}

int SetupTCPServerSocket(unsigned short port){
    
    //Create socket for incoming connections
    int sock;//Socket descriptor for server
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))< 0)
        DieWithSystemMessage("socket() failed");
    
    //Construct local address structure
    struct sockaddr_in servAddr;                   //Local address
    memset(&servAddr, 0, sizeof(servAddr));        //Zero out structure
    servAddr.sin_family = AF_INET;                 //IPv4 address family
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);  //Any incoming interface
    servAddr.sin_port = htons(port);               //Local port
    
    //Bind to the local address
    if(bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
        DieWithSystemMessage("bind() failed");
    
    //Mark the socked so it will listen for incoming conncetions
    if(listen(sock, MAXPENDING) < 0)
        DieWithSystemMessage("listen() failed");
    
    return sock;
}

int AcceptTCPConnection(int servSock) {
   
    struct sockaddr_in clntAddr; //Client address
    //Set length of client address structure (in-out parameter)
    socklen_t clntAddrLen = sizeof(clntAddr);
    
    //Wait for a client to connect
    int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
    if(clntSock < 0)
        DieWithSystemMessage("accept() failed");
    
    //clntSock is now connected to a client
    
    char clntName[INET_ADDRSTRLEN]; //String to contain client address
    if(inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr,clntName, sizeof(clntName)) != NULL)
        printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));
    else
        puts("Unable to get client address");
    
    return clntSock;

}


void geoAppProtocol(int clntSocket) {
  
   
   int nAPs;

   FILE *instream = fdopen(clntSocket, "r+");
   if(instream == NULL)
        DieWithSystemMessage("fdopen() failed");
   
   
   int numNAPs = fread(&nAPs,4,1, instream);
   printf("Number nAPs read: %d\n", numNAPs);
   printf("nAPs read: %d\n", nAPs);
   
   APINFO rcv[nAPs];
   
   for(int i=0; i< nAPs; i++){
   fread(&rcv[i].ssid, sizeof(char) ,32, instream);
   fread(&rcv[i].mac, sizeof(char) ,18, instream);
   fread(&rcv[i].rssi, 4,1, instream);
   	    
   
   }
   //if(fread(rcv, sizeof(char) ,12, instream) != 12){
   //		DieWithSystemMessage("fread() failed");
   //	}
   	
   int numSSID, numMAC, numRSSI; 
   numSSID = numMAC = numRSSI = 0;
   
     	for(int i=0; i<nAPs; i++){
   printf("Number SSID%d read: %d\n",i, numSSID);
   printf("SSID%d read: %s\n",i, rcv[i].ssid);
   printf("Number MAC%d read: %d\n",i, numMAC);
   printf("MAC%d read: %s\n",i, rcv[i].mac);
   printf("Number RSSI%d read: %d\n",i, numRSSI);
   printf("RSSI%d read: %d\n",i, rcv[i].rssi);
   	}
   fclose(instream);//close the socket connection
   
   
}


int main(int argc, char *argv[]){
    
    if(argc != 2)//Test for correct number of arguments
        DieWithUserMessage("Parameter(s)", "<Server Port>");
    
    in_port_t servPort = atoi(argv[1]); //First arg: local port
    
    int servSock = SetupTCPServerSocket(servPort);
        
    while(1) {        
        int clientSock = AcceptTCPConnection(servSock);
        geoAppProtocol(clientSock);
        
    }
    
    //NOT REACHED
}
