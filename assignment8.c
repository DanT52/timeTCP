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
#define MY_PORT_NUMBER_STR "49999"


int client_connect(char const* address){
    printf("This is the client. Address: %s\n", address);

    int sockfd;
    struct addrinfo hints;
    struct addrinfo *actualdata;
    memset(&hints, 0, sizeof(hints));
    int err;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    err = getaddrinfo(address, MY_PORT_NUMBER_STR, &hints, &actualdata);
    sockfd = socket(actualdata->ai_family, actualdata->ai_socktype, 0);
    connect(sockfd, actualdata->ai_addr, actualdata->ai_addrlen);


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


