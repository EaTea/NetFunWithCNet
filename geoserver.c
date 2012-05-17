#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // for memset()
#include <unistd.h>      // for close()
#include <sys/socket.h>  // for socket(), connect(), send(), and recv()
#include <sys/types.h>
#include <arpa/inet.h>   // for sockaddr_in and inet_addr()
#include <netinet/in.h>

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
  
   
    char buffer[BUFSIZ]; //Buffer for IPOD request 
    
    //Receive request from IPOD
    
    ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZ, 0);
    if(numBytesRcvd < 0)
        DieWithSystemMessage("recv() failed");
    ssize_t numBytesRcvdTotal = numBytesRcvd;
    
    while(numBytesRcvd > 0) {
        numBytesRcvd = recv(clntSocket, buffer, BUFSIZ, 0);
        if( numBytesRcvd < 0)
            DieWithSystemMessage("rcvd() failed");
        numBytesRcvdTotal = numBytesRcvdTotal + numBytesRcvd;
    }
    printf("Message: %s\n", buffer);
    
    
    /*
    int8_t numSamples;
    
    ssize_t numBytesRcvd = recv(clntSocket,(void*)(&numSamples),1, 0);
    if(numBytesRcvd < 0)
        DieWithSystemMessage("recv() failed");
    
    printf("Number of Samples to receive: %d\n", numSamples);

    
    IPOD_SAMPLE samples[numSamples]; 
    //wrap the socket in an input stream
    FILE *instream = fdopen(clntSocket, "r");
    if(instream == NULL)
        DieWithSystemMessage("fdopen() failed");
    
    for(int i=0; i<numSamples; i++){
        if(fread(&samples[i], sizeof(IPOD_SAMPLE), 1, instream) != 1)
            DieWithSystemMessage("fread() failed");
    }
    
    fclose(instream); //close the socket connection
    */
    
    //TODO: Collate and Calculate (lat, lon)
    
    
}


int main(int argc, char *argv[]){
    
    if(argc != 2)//Test for correct number of arguments
        DieWithUserMessage("Parameter(s)", "<Server Port>");
    
    in_port_t servPort = atoi(argv[1]); //First arg: local port
    
    int servSock = SetupTCPServerSocket(servPort);
        
    while(1) {        
        int clntSock = AcceptTCPConnection(servSock);
        geoAppProtocol(clntSock);
        
    }
    
    //NOT REACHED
}
