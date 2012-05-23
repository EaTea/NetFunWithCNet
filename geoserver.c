#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // for memset()
#include <unistd.h>      // for close()
#include <sys/socket.h>  // for socket(), connect(), send(), and recv()
#include <sys/types.h>
#include <netinet/in.h>
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

float ntohf(uint32_t p)
{
    float f = ((p>>16)&0x7fff); // whole part
    f += (p&0xffff) / 65536.0f; // fraction
    
    if (((p>>31)&0x1) == 0x1) { f = -f; } // sign bit set
    
    return f;
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
  
   FILE *instream = fdopen(clntSocket, "r+");
   if(instream == NULL)
        DieWithSystemMessage("fdopen() failed");
   
    IPOD_SAMPLE sample;
    
    
    if( fread(&sample.nAPs, sizeof(int), 1, instream)!= 1){
        DieWithSystemMessage("fread() failed\n");
    }
    
    fread(&sample.lat, sizeof(sample.lat),1,instream);
    fread(&sample.lon, sizeof(sample.lon),1,instream);
    
    sample.nAPs = ntohl(sample.nAPs);
    sample.lat =ntohf(sample.lat);
    sample.lon =ntohf(sample.lon);
    printf("Number of APs: %d\n",sample.nAPs);
    printf("Latitude, Longitude: %f %f\n", sample.lat, sample.lon);

    
    sample.APs = (APINFO*)malloc(sample.nAPs*sizeof(APINFO));
   
    for(int i=0; i<sample.nAPs; i++){
    fread(&sample.APs[i].ssid, sizeof(char) ,32, instream);
    fread(&sample.APs[i].mac, sizeof(char) ,18, instream);
    fread(&sample.APs[i].rssi, sizeof(int32_t),1, instream);	    
    }
    fclose(instream);//close the socket connection
   
    for(int i=0; i<sample.nAPs; i++){
    printf("AP %d SSID: %s\n",i, sample.APs[i].ssid);
   	printf("AP %d MAC: %s\n",i, sample.APs[i].mac);
   	printf("AP %d RSSI: %d\n",i,sample.APs[i].rssi);
    }
}

void SendImage(int clntSocket){
    
    
    const char *filename = "/Users/20356245/Desktop/sampleimage.png";
    FILE *outstream = fdopen(clntSocket, "wb");
    FILE *image2send = fopen(filename, "rb");
    printf("Opened  both files\n");
    
    
    fseek(image2send, 0L, SEEK_END);
 	int imageNBytes = ftell(image2send);
    fseek(image2send, 0L, SEEK_SET);
    
    
    
    int NETWORKimageNBytes = htonl(imageNBytes);
    if(fwrite( &NETWORKimageNBytes,sizeof(NETWORKimageNBytes),1, outstream) != 1){
        printf("fwrite() imageNBytes failed\n");
    }
    
    printf("imageNBytes Written: %d\n", imageNBytes);
    char buffer[imageNBytes];
    fread(buffer, sizeof(char), imageNBytes, image2send);
    
    printf("fopened() filename\n");
    int numbat = fwrite(buffer, sizeof(char), imageNBytes, outstream);
    if(numbat!= imageNBytes){
        printf("fwrite() FAILED!!!!: wrote %d bytes\n", numbat);
    }
    printf("fwrote() image\n");
    fclose(image2send);
    fclose(outstream);
    printf("fclosed() both streams\n");
    
}

int main(int argc, char *argv[]){
    
    if(argc != 2)//Test for correct number of arguments
        DieWithUserMessage("Parameter(s)", "<Server Port>");
    
    in_port_t servPort = atoi(argv[1]); //First arg: local port
    
    
    //TODO: request area from photoserver and intitialise etc.
    
    int servSock = SetupTCPServerSocket(servPort);
        
    while(1) {        
        int clientSock = AcceptTCPConnection(servSock);
        
        
        
        
        geoAppProtocol(clientSock);
        SendImage(clientSock);
    }
    
    //NOT REACHED
}
