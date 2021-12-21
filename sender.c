#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <netinet/tcp.h>
#define MAX 1024
#define IP "127.0.0.1"
#define PORT 7900

void sendFile(int sock,int i){
    char value[MAX]={0};
    FILE* file;
    int send_file,bytes;
    file=fopen("1mb.txt" , "r");
    if(file==NULL)
    {
        perror("file does not exist\n");
        exit(1);
    }
    bytes=0;
    while(fread(value,sizeof(char),MAX,file)==MAX)
    {
        send_file=send(sock,value,sizeof(value),0);
        if(send_file==-1)
        {
            perror("error while sending the file");
            exit(1);
        }
        bytes+=send_file;
    }

    printf("upload file number %d - size of %d bytes\n" , i+1,bytes);
}

int main(){

//////////////////////Create TCP socket//////////////////////////////
    int sock , sock_connect;
    struct sockaddr_in socket_address;
    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<=0)
    {
        perror("problem with the creation of the sock\n");
        exit(1);
    }
    printf("successfully created socket\n");
        memset(&socket_address, '\0', sizeof(socket_address));

///////////////////////////Connection to server//////////////////////
  socket_address.sin_family = AF_INET;
	socket_address.sin_port =htons(PORT);
	socket_address.sin_addr.s_addr = inet_addr(IP);

//////////////////////Sends 5 times before changing/////////////////////
    for (int i=0;i<5;i++)
    {
        sock_connect=connect(sock,(struct sockaddr*) &socket_address,sizeof(socket_address));
        if(sock_connect!=0)
        {
            perror("problem while making connection");
            exit(1);
        }
        if(i==0)
    	   printf("successfuly connected to server\n\n");
        sendFile(sock,i);
        close(sock);
        sock=socket(AF_INET,SOCK_STREAM,0);
        if(sock<=0)
        {
            perror("problem with the creation of the socket\n");
            exit(1);
        }
    }

////////////////////////////Algorithm changing///////////////////////////
    char buf[256];
    socklen_t len;
    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP,TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        return -1;
    }
     printf("5 files are successfully sent in a %s algorithm\n", buf);
    char* reno="reno";
    char* cubic="cubic";
    if(strcmp(buf,cubic)==0)
    {
        strcpy(buf,reno);
    }
    else
        strcpy(buf,cubic);

    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
    {
        perror("setsockopt");
        return -1;
    }

    printf("\n***** Algorithm changed to %s *****\n\n" , buf);

////////////////////////////////////Sends 5 times after changing/////////////////////
    for (int i=0;i<5;i++)
    {
        sock_connect=connect(sock,(struct sockaddr*) &socket_address,sizeof(socket_address));
        if(sock_connect!=0)
        {
            perror("problem while making connection");
            exit(1);
        }
        sendFile(sock,i);
        close(sock);
        sock=socket(AF_INET,SOCK_STREAM,0);
        if(sock<=0)
        {
            perror("problem with the creation of the sock\n");
            exit(1);
        }
        if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
        {
            perror("setsockopt");
            return -1;
        }
    }
    printf("5 files are successfuly sent in a %s algorithm\n", buf);
    close(sock);
    printf("client disconnected from server\n");

}
