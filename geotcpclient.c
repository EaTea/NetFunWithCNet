#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // for memset()
#include <unistd.h>      // for close()
#include <sys/socket.h>  // for socket(), connect(), send(), and recv()
#include <sys/types.h>
#include <arpa/inet.h>   // for sockaddr_in and inet_addr()
#include <netinet/in.h>




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
    
    char *servIP = "130.95.1.70";
    char *echoString = "area \r\n";
    
    in_port_t servPort = 3230;
    
    int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sock<0){
        DieWithSystemMessage("socket() failed");
    }
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if(rtnVal == 0){
        DieWithUserMessage("inet_pton() failed", "invalid address string");
    }else{ if(rtnVal < 0){
        DieWithSystemMessage("inet_pton() failed");
    }
    }

    servAddr.sin_port = htons(servPort);
    
    if(connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr))<0){
                DieWithSystemMessage("connect() failed");
        }
           
    size_t echoStringLen = strlen(echoString);
    
    ssize_t numBytes = send(sock, echoString, echoStringLen, 0);
    if(numBytes <0)
        DieWithSystemMessage("send() failed");
    else if(numBytes != echoStringLen)
        DieWithUserMessage("send()", "sent unexpected number of bytes");
    
    unsigned int totalBytesRcvd = 0;
    fputs("Received: ", stdout);
       
    while(totalBytesRcvd < 49) {
        char buffer[BUFSIZ];
        numBytes = recv(sock, buffer, BUFSIZ-1, MSG_CONFIRM);
        if(numBytes < 0)
            DieWithSystemMessage("recv() failed");
        else if(numBytes==0)
            DieWithUserMessage("recv()", "connection closed prematurely");
        totalBytesRcvd += numBytes;
        buffer[numBytes] = '\0';
        fputs(buffer, stdout);
    }
    
    fputc('\n',stdout);
    
    close(sock);
    exit(0);
          
}