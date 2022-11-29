#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/syscall.h> //thread_id

#define PORT 7507
#define BUFFER_SIZE 2048
#define BACKLOG 10
#define gettid() ((pid_t)syscall(SYS_gettid))

typedef struct pthread_args {
	int sockfd;
	struct sockaddr_in client_address;
} pthread_args;

void* handler(void* args) {
	pthread_args* pthread_arg = (pthread_args*)args;
	int sockfd = pthread_arg->sockfd;
	struct sockaddr_in client_address = pthread_arg->client_address;
	char buffer[BUFFER_SIZE];
	char message_sent[2*BUFFER_SIZE];
	clock_t start, end, duration;
	
	free(args);
	
	bzero(buffer, BUFFER_SIZE);
	bzero(message_sent, BUFFER_SIZE);
	
	while(1) {
		start = clock();
			
		if((read(sockfd, &buffer, BUFFER_SIZE-1))<0) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		
		printf("Mesajul primit din partea clientului (thread id=%d): %s\n", gettid(), buffer);
		
		char* copied_message = strdup(buffer);
		char* p;		
		
		p = strtok(copied_message, " ");
		if(!strcmp(p, ".d"))
		{
			p=strtok(NULL, " ");
			if(p) {
				int val = atoi(p);
				if(val < 0) {
					if((write(sockfd, "Eroare: delay-ul nu poate sa fie negativ.", 42))<0)
					{
						perror("write");
						exit(EXIT_FAILURE);
							
					}
				}	
				else { 
					//tba: add delay to the message
				}		
			}
			else {
				if((write(sockfd, "Eroare: nu ati introdus durata delay-ului.\nPentru a seta delay: .d <durata>", 76))<0)
				{
					perror("write");
					exit(EXIT_FAILURE);
				}
			}
			bzero(buffer, sizeof(buffer));
		}
		
		else {
			if(!strcmp(buffer, "stop") || !strcmp(buffer, "STOP")) {
				if((write(sockfd, "Conexiunea a fost inchisa.", 27))<0) {
					perror("write");
					exit(EXIT_FAILURE);
				}
			printf("\nServer: am primit mesajul STOP din partea clientului (thread id=%d), acesta va fi deconectat de la server.\n", gettid());
			close(sockfd);
			return NULL;	
			}
		
			end = clock();
		
			duration = (end-start);
		
			sprintf(message_sent, "Durata de transmitere/receptie a mesajului \"%s\" este de %f secunde.", buffer, (float)duration/CLOCKS_PER_SEC);
			
			bzero(buffer, sizeof(buffer));
			
			if((write(sockfd, message_sent, strlen(message_sent)))<0) {
				perror("write");
				exit(EXIT_FAILURE);
			}
			
			bzero(message_sent, sizeof(message_sent));
		}
		free(copied_message);
	}
}

int main() {
	int sockfd, new_sockfd, bytes;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in server_address;
	
	pthread_attr_t pthread_attr;
	pthread_args* pthread_arg;
	pthread_t thread;
	
	socklen_t client_len;
	
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
    		
    		pthread_arg->sockfd = new_sockfd;
    		
    		if(pthread_create(&thread, &pthread_attr, handler, (void*)pthread_arg) != 0) {
    			perror("pthread_create");
    			free(pthread_arg);
    			exit(EXIT_FAILURE);
    		}
    		
    		printf("Server: Un client s-a conectat la server.\n\n");
    	}
	
	close(sockfd);
	return 0;
}
