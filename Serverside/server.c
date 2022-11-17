#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>

#define PORT 7507
#define BUFFER_SIZE 2048
#define BACKLOG 10

typedef struct pthread_args {
	int sockfd;
	struct sockaddr_in client_address;
	
	char* message; //mesajul trimis de catre client
	clock_t start, stop; //pentru masurarea duratei de transmitere
	
	float seconds;
	float delay; //initial este setat pe 0, daca se trimite .d <durata> ca si argument, va fi modificat
} pthread_args;

void* handler(void* args) {
	pthread_args* pthread_arg = (pthread_args*)args;
	int sockfd = pthread_arg->sockfd;
	struct sockaddr_in client_address = pthread_arg->client_address;
	float seconds = pthread_arg->seconds;
	float delay = pthread_arg->delay;
	
	free(args);
	
	//tba: send/write with the client, counting the duration of sending/receiving message
	//and maybe signal handlers for CTRL-Z and CTRL-C to shutdown the socket without having to send a message like "stop"
	
	
	close(sockfd);
	return NULL;	
}

int main() {
	int sockfd, new_sockfd;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in server_address;
	
	pthread_attr_t pthread_attr;
	pthread_args* pthread_arg;
	pthread_t thread;
	
	socklen_t client_len;
	
	//initializare adresa IPv4
	memset(&server_address, 0, sizeof(server_address));
    	server_address.sin_family = AF_INET;
    	server_address.sin_port = htons(PORT);
    	server_address.sin_addr.s_addr = INADDR_ANY;
    	
    	//socket
    	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
       		perror("socket");
        	exit(EXIT_FAILURE);
    	}
    	
    	//bind
    	if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
      		perror("bind");
      		exit(EXIT_FAILURE);
   	}
   	
   	//listen
   	if (listen(sockfd, BACKLOG) == -1) {
        	perror("listen");
        	exit(EXIT_FAILURE);
    	}
    	
    	//vom folosi threaduri detached, la care nu mai trebuie sa dam join
    	if (pthread_attr_init(&pthread_attr) != 0) {
      	  	perror("pthread_attr_init");
      		exit(EXIT_FAILURE);
    	}
    	
  	if (pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED) != 0) {
        	perror("pthread_attr_setdetachstate");
        	exit(EXIT_FAILURE);
    	}
    	
    	while(1) {
    		pthread_arg = (pthread_args*)malloc(1*sizeof(pthread_args*));
    		
    		if(!pthread_arg) {
    			perror("malloc");
    			exit(EXIT_FAILURE);
    		}
    		
    		//accept
    		client_len = sizeof(pthread_arg->client_address);
    		new_sockfd = accept(sockfd, (struct sockaddr*)&pthread_arg->client_address, &client_len);
    		if(new_sockfd == -1) {
    			perror("accept");
    			free(pthread_arg);
    			exit(EXIT_FAILURE);
    		}
    		
    		read(new_sockfd, buffer, sizeof(buffer));
    		
    		pthread_arg->sockfd = new_sockfd;
    		pthread_arg->seconds = 0;
    		pthread_arg->delay = 0;
    		
    		if(pthread_create(&thread, &pthread_attr, handler, (void*)pthread_arg) != 0) {
    			perror("pthread_create");
    			free(pthread_arg);
    			exit(EXIT_FAILURE);
    		}
    	}
	
	//shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	return 0;
}
