#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
/*
   struct hostent{
   char *h_name;          //Official name of host
   char **h_aliases; 		//List of alternate names for host 
   int h_addrtype;			//domain type
   int h_length;				//length of address
   char **h_addr_list;				//list of addresses from name server
# define h_addr h_addr_list[0]	  //backward compatibility
};
 */
typedef struct{
	char a;
	int b;
	float c;
} Packet;

Packet packet;
 
void error(char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc,char *argv[]) {
	int sockfd,portno,n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	if(argc<4) {
		fprintf(stderr,"usage %s host port",argv[0]);
		exit(0);	
	}
	portno = atoi(argv[2]);
	char ch = argv[3][0];
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if(server == NULL ) {
		fprintf(stderr,"ERROR, no such host");
		exit(0);
	}
	bzero((char *) &serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
	if( connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0 ) 
		error("ERROR connecting");
	if(ch=='s') {
		n = read(sockfd,&packet,sizeof(packet));
		if(n<0)
			error("ERROR reading from socket");
		printf(">>Received Message : %c %d %f\n",packet.a,packet.b,packet.c);
	} else {
		bzero(buffer,256);
		n = read(sockfd,buffer,255);
		if(n<0)
			error("ERROR reading from socket");
		printf(">>Recieved message : %s ",buffer);
	}
	return 0;
}
