

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

float receiveFile(int sock ,int i){
struct timespec start, end;
    int sum=0, recive;
    char buff[MAX]={0};
    clock_gettime(CLOCK_REALTIME, &start);
    while(1)
    {
        recive=recv(sock,buff,MAX,0);
        sum+=recive;
        if(recive==-1)
        {
        	perror("can't receive file!\n");
        	exit(1);
        }
        if(recive==0)
        {
		    printf("read file number %d - size of %d bytes\n" , i, sum);
           	break;
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
}


int main()
{
    socklen_t address_len;
    int sock , client_socket, client_bind ,client_listen;
    double dCubic,avgReno;
    struct sockaddr_in socket_address , receive_address;
    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("problem with the creation of the socket");
        exit(1);
    }
    printf("successfuly created socket\n");
        memset(&socket_address, '\0', sizeof(socket_address));
 	socket_address.sin_family = AF_INET;
	socket_address.sin_port =htons(PORT);
	socket_address.sin_addr.s_addr = inet_addr(IP);
	client_bind=bind(sock, (struct sockaddr*)&socket_address, sizeof(socket_address));
    if(client_bind<0)
    {
        perror("problem with the creation of bind\n");
        exit(1);
    }
    printf("successfuly made bind\n");
    float sumForCubic=0;

    client_listen=listen(sock,10);
    if(client_listen==-1)
    {
        perror("problem with the listening\n");
        exit(1);
    }
    printf("listening...\n");
    for(int i=0;i<5;i++)
    {
        address_len=sizeof(receive_address);
        client_socket=accept (sock,(struct sockaddr*) &receive_address,&address_len);
        if (i==0)
        {
            printf("connected to client\n\n");
        }
        dCubic= receiveFile(client_socket,i+1);
        sumForCubic+=dCubic;
        close(client_socket);
    }
    double avgCubic=sumForCubic/5;

    //////////////////////////changing from cubic to reno

    char buf[256];
    socklen_t len;
    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP,TCP_CONGESTION, buf, &len) != 0)
    {
        perror("problem with getsockopt");
        return -1;
    }
    printf("the avg for %s Algorithm is: %lf seconds\n" ,buf,avgCubic);

    char* reno="reno";
    char* cubic="cubic";
    if(strcmp(buf,cubic)==0)
    {
        strcpy(buf,reno);
    }
    else{
        strcpy(buf,cubic);
    }
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
    {
        perror("problem with setsockopt");
        return -1;
    }

    printf("\n***** Algorithm changed to %s *****\n\n" , buf);
    //////////////////////////


    float sumForReno=0;
    float dReno;
    for(int i=0;i<5;i++)
    {
        address_len=sizeof(receive_address);
        client_socket=accept (sock,(struct sockaddr*) &receive_address,&address_len);
        if (setsockopt(client_socket, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
        {
        	perror("setsockopt");
        	return -1;
    	}
        dReno= receiveFile(client_socket,i+1);
        sumForReno+=dReno;
        close(client_socket);
    }
    avgReno=sumForReno/5;
    printf("the avg for %s Algorithm is: %lf seconds\n" ,buf,avgReno);

    close(sock);
    printf("server disconnected from client\n");
}
