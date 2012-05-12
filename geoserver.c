#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // for memset()
#include <unistd.h>      // for close()
#include <sys/socket.h>  // for socket(), connect(), send(), and recv()
#include <sys/types.h>
#include <arpa/inet.h>   // for sockaddr_in and inet_addr()
#include <netinet/in.h>

static const int MAXPENDING = 5; //Max. outstanding connection requests

void geoAppProtocol(int clnSocket) {
    
    char buffer[BUFSIZE]; //Buffer for IPOD message string
    
    //Receive message from IPOD
    
    
    
    
}






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

int main(int argc, char *argv[]){
    
    if(argc != 2)//Test for correct number of arguments
        DieWithUserMessage("Parameter(s)", "<Server Port>");
    
    in_port_t servPort = atoi(argv[1]); //First arg: local port
    
    //Create socket for incoming connections
    int servSock;//Socket descriptor for server
    if((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))< 0)
        DieWithSystemMessage("socket() failed");
    
    //Construct local address structure
    struct = sockaddr_in servAddr;                 //Local address
    memset(&servAddr, 0, sizeof(servAddr));        //Zero out structure
    servAddr.sin_family = AF_INET;                 //IPv4 address family
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);  //Any incoming interface
    servAddr.sin_port = htons(servPort);           //Local port
    
    //Bind to the local address
    if(bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
        DieWithSystemMessage("bind() failed");
    
    //Mark the socked so it will listen for incoming conncetions
    if(listen(servSock, MAXPENDING) < 0)
        DieWithSystemMessage("listen() failed");
    
    while(1) { //Run forever
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
        
        geoAppProtocol(clntSock);
    }
    
    //NOT REACHED
}