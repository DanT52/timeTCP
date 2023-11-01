#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

#define MY_PORT_NUMBER 49999



int client_connect(char const* address){
    printf("This is the client. Address: %s\n", address);

    int sockfd;
    struct sockaddr_in servAddr;
    struct hostent* hostEntry;
    struct in_addr **pptr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(MY_PORT_NUMBER);
    
    hostEntry = gethostbyname("localhost");
    pptr = (struct in_addr **) hostEntry->h_addr_list;
    memcpy(&servAddr.sin_addr, *pptr, sizeof(struct in_addr));

    if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        perror("connect");
        exit(1);
    }

    char buffer[256];
    read(sockfd, buffer, sizeof(buffer));
    printf("%s", buffer);

    close(sockfd);
    return 0;

}

int start_server(){

    printf("This is the server.\n");

    int listenfd, connectfd;

    struct sockaddr_in servAddr, clientAddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);


    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(MY_PORT_NUMBER);
    servAddr.sin_addr.s_addr = htons(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){
        perror("bind");
        exit(1);
    }

    listen(listenfd, 1);

    int length = sizeof(clientAddr);

    connectfd = accept(listenfd, (struct sockaddr *) &clientAddr, &length);

    if (fork() == 0) {
        close(listenfd);

        char hostName[NI_MAXHOST];
        int hostEntry;

        hostEntry = getnameinfo((struct sockaddr*)&clientAddr, sizeof(clientAddr), hostName, sizeof(hostName), NULL, 0, NI_NUMERICSERV);
        printf("hostname: %s\n", hostName);

        time_t rawtime;
        struct tm * timeinfo;
        char buf[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buf, 80, "%a %b %d %H:%M:%S", timeinfo);
        buf[18] = '\n';

        write(connectfd, buf, 19);
        close(connectfd);
        exit(0);

    }


    exit(0);
}

int main(int argc, char const *argv[]){
    // Check the number of command line arguments
    if (argc < 2 || argc > 3) {
        printf("Usage: %s client address\nUsage: %s server\n",argv[0], argv[0]);
        return 1;
    }

    // Check if the first argument is "client" and there are three arguments in total
    if (strcmp(argv[1], "client") == 0 && argc == 3) {
        client_connect(argv[2]);
    }
    // Check if the first argument is "server" and there are two arguments in total
    else if (strcmp(argv[1], "server") == 0 && argc == 2) {
        start_server();
    }
    else {
        printf("Usage: %s client address\nUsage: %s server\n",argv[0], argv[0]);
        return 1;
    }

    return 0;
}


