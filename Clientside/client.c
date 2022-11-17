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
	
	char message[] = "Hello from the client!";
	
	if(argc>1) {
		printf("Numar invalid de argumente.\n");
		printf("Pentru a a va conecta la server: ./run_client\n");
		printf("Dupa ce s-a realizat conexiunea, puteti sa trimiteti mesaje.\n");
		printf("Sau puteti sa setati un delay la transmitere tastand: .d <durata>.\n");
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
	
	//tba: sending messages from the client and receiving answer from the server
	
	close(sockfd);
	return 0;
}
