#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 7507
#define SERVER "localhost"
#define BUFFER_SIZE 2048

int main(int argc, char *argv[])
{
	int sockfd;
	
	struct hostent* server_host;
	struct sockaddr_in server_address;
	
	char message_sent[BUFFER_SIZE];
	char message_received[2*BUFFER_SIZE];
	
	if(argc>1) {
		printf("Numar invalid de argumente.\n");
		printf("Pentru a a va conecta la server: ./run_client\n");
		printf("Dupa ce s-a realizat conexiunea, puteti sa trimiteti mesaje.\n");
		printf("Sau puteti sa setati un delay la transmitere tastand: .d <durata>.\n");
		exit(EXIT_FAILURE);
	}
	
	server_host = gethostbyname(SERVER);
	
	//initializare adresa IPv4
	memset(&server_address, 0, sizeof(server_address));
    	server_address.sin_family = AF_INET;
    	server_address.sin_port = htons(PORT);
    	memcpy(&server_address.sin_addr.s_addr, server_host->h_addr, server_host->h_length);
    	
    	//socket
    	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
       		perror("socket");
        	exit(EXIT_FAILURE);
    	}
    	
    	//connect
    	if (connect(sockfd, (struct sockaddr *)&server_address, sizeof server_address) == -1) {
		perror("connect");
       		exit(EXIT_FAILURE);
	}
	
	printf("Conectat la server.\n");
	
	//loop pentru a citi mesajele de la server
	while(1) {
		bzero(message_sent, sizeof(message_sent));
		printf("Introduceti mesajul: ");
		fgets(message_sent, BUFFER_SIZE-1, stdin);
		message_sent[strcspn(message_sent, "\n")]=0;
		
		//send
		if((write(sockfd, &message_sent, strlen(message_sent)))<0) {
			perror("write");
			exit(EXIT_FAILURE);
		}
	
		bzero(message_received, sizeof(message_received));
		
		//read
		if ((read(sockfd, &message_received, (2*BUFFER_SIZE)-1))<0)
		{
			perror("read");
			exit(EXIT_FAILURE);
		}
		
		if(!strcmp(message_received, "Conexiunea a fost inchisa.")) {
			printf("Ati fost deconectat de la server.\n");
			break;
		}
	
		printf("Serverul spune: %s\n", message_received);
	}
	
	close(sockfd);
	return 0;
}
