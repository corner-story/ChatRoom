#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#define SERVER_ADDR "39.107.83.159"
#define SERVER_PORT 6666

int sockfd;

void *read_user_input(void *sockfd);
void *recv_from_server(void *sockfd);
void sigint_handler(int num);

int main(int argc, char *argv[]){
    // set sigint handler
    signal(SIGINT, sigint_handler);

    // create a socket  
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd != -1);

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    addr.sin_port = htons(SERVER_PORT);

    int res = connect(sockfd, (struct sockaddr*)&addr, addrlen);
    assert(res != -1);

    pthread_t p1, p2;
    assert(pthread_create(&p1, NULL, read_user_input, &sockfd) == 0);
    assert(pthread_create(&p2, NULL, recv_from_server, &sockfd) == 0);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    return 0;
}

void *read_user_input(void *sockfd){
    int fd = *((int*)sockfd);
    char buffer[1024];
    char *res = NULL;
    while((res = fgets(buffer, 1024, stdin)) != NULL){
        write(fd, buffer, strlen(res));
    }
}


void *recv_from_server(void *sockfd){
    int fd = *((int *)sockfd);
    char buffer[1024];
    int n = -1;
    while((n = read(fd, buffer, 1024)) > 0){
        write(1, buffer, n);
    }
}

void sigint_handler(int num){
    close(sockfd);
    exit(0);
}