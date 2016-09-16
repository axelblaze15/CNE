#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void readclient1(int); 
void writeclient2(int,char);
char buffer[256];

typedef struct{
	char a;
	int b;
	float c;
} Packet;

Packet packet;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}


char ch;

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, new2sockfd, portno, pid;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr,cli_addr2;
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	if(argc < 3) {
		fprintf(stderr,"ERROR, c for char, i for integer, f for float, s for struct\n");
		exit(1);
	}
	ch = argv[2][0];
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	listen(sockfd,10);
	clilen = sizeof(cli_addr);
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			error("ERROR on accept");
			close(sockfd);
			exit(0);
		}
		readclient1(newsockfd);
		close(newsockfd);
		new2sockfd = accept(sockfd, (struct sockaddr *)&cli_addr2, &clilen);
		if(new2sockfd < 0) {
			error("ERROR on write");
			close(sockfd);
			exit(0);
		}
		if(argv[2][0]=='c') {
			writeclient2(new2sockfd,'c');
		} else
		if(argv[2][0]=='i') {
			writeclient2(new2sockfd,'i');
		} else
		if(argv[2][0]=='f') {
			writeclient2(new2sockfd,'f');
		} else
		if(argv[2][0]=='s') {
			writeclient2(new2sockfd,'s');
		}
		else
			fprintf(stderr,"ERROR, just enter the correct code....\n");
		close(new2sockfd);
	}
	close(sockfd);
	return 0; 
}

void writeclient2(int sock,char c)
{
	int n,i;
	if(c=='c') {
		for(i=0;buffer[i]!='\0';i++) {
			buffer[i]--;
		}
	}
	else
	if(c=='i') {
		int x = atoi(buffer);
		x--;
		sprintf(buffer,"%d",x);
	}
	else
	if(c=='f') {
		float x = atof(buffer);
		x*=1.5;
		sprintf(buffer,"%f",x);
	}
	else
	if(c=='s') {
		packet.a = packet.a-1;
		packet.b = packet.b*2;
		packet.c = packet.c*3;
		n = write(sock, &packet,sizeof(packet));
		if(n<0)
			error("ERROR writing to socket");
		n = read(sock,buffer,255);
		if(n<0)
			error("ERROR reading from socket");
		printf(">>Message recieved from client1...now sending to client2\n");
		return;
	}
	n = write(sock,buffer,255);
	if(n<0)
		error("ERROR writing to socket");
	bzero(buffer,256);
	n = read(sock,buffer,255);
	if(n<0)
		error("ERROR reading from socket");
	printf(">>Message recieved from client 1...now sending to client\n");
}

void readclient1(int sock)
{
	if(ch=='s') {
		int n = read(sock,&packet,sizeof(packet));
		if(n<0)
			error("ERROR reading from socket");
		printf(">>Here is the message: %c %d %f\n",packet.a,packet.b,packet.c);
		n = write(sock,"I got your message",18);
		if(n<0)
			error("ERROR writing to socket");
		return;
	}
	int n;     
	bzero(buffer,256);
	n = read(sock,buffer,255);
	if (n < 0) error("ERROR reading from socket");
	printf("Here is the message: %s\n",buffer);
	n = write(sock,"I got your message",18);
	if (n < 0) 
		error("ERROR writing to socket");
}
