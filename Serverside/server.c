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

#define MYPORT 7507
#define BUFFER_SIZE 255
#define BACKLOG 10

int main(void)
{
    int sockfd; //socket server
    int client_fd; //socket client
    int sin_size;
    int yes=1;
    
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), '\0', 8);

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))
                                                                    == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    
    printf("Serverul este deschis.\n");

    while(1) {  
        sin_size = sizeof(struct sockaddr_in);
        if ((client_fd = accept(sockfd, (struct sockaddr *)&their_addr,
                                                        &sin_size)) == -1) {
            perror("accept");
            continue;
        }
        printf("server: conexiune de la: %s\n",
        inet_ntoa(their_addr.sin_addr));
        char message[] = "connected";
            if (send(client_fd, message, sizeof(message), 0) == -1)
                perror("send");
        close(client_fd);
    }
    return 0;
}
