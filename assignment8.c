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

//client connect function
//attempts to connect to the provided address
// expects to recive something back (the time 19 chars long)
int client_connect(char const* address){

    int sockfd; //socket file descriptor
    struct addrinfo hints;
    struct addrinfo *actualdata;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    // go from address to addrinfo struct
    int err = getaddrinfo(address, MY_PORT_NUMBER_STR, &hints, &actualdata);
    if (err != 0){
        fprintf(stderr,"Error: %s\n", gai_strerror(err));
        exit(1);
    }
    //create socket
    sockfd = socket(actualdata->ai_family, actualdata->ai_socktype, 0);
    if (sockfd == -1) {
        fprintf(stderr, "Error: Socket creation, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
        exit(1);
    }

    //connect to server with socket
    if (connect(sockfd, actualdata->ai_addr, actualdata->ai_addrlen) == -1){
        fprintf(stderr, "Error: Connecting, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
        exit(1);
    }
    //done with addrinfo
    freeaddrinfo(actualdata);

    char buffer[20];
    int bytes_read = read(sockfd, buffer, sizeof(buffer));

    if (bytes_read == -1) {
        fprintf(stderr, "Error: Reading from socket, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
        close(sockfd);
        exit(1);
    }
    buffer[19] = '\0'; //terminate string
    printf("%s", buffer);
    if (close(sockfd) == -1) {
        fprintf(stderr, "Error: Closing socket, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
        exit(1);
    }

    exit(0); //sucess
}


//handles a client connecting to server / the server response
//sends back the time to the client
//logs number of total connections the client name.
void server_response(int connectfd, struct sockaddr_in clientAddr, int listenfd, int times_connected) {

    if (close(listenfd) == -1) fprintf(stderr, "Error: Closing listen socket, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);

    char hostName[NI_MAXHOST];
    int hostEntry = getnameinfo((struct sockaddr*)&clientAddr, sizeof(clientAddr), hostName, sizeof(hostName), NULL, 0, NI_NUMERICSERV);

    if (hostEntry != 0) {
        fprintf(stderr, "Error: getnameinfo, STRERR: %s, ERRNO: %d\n", gai_strerror(hostEntry), hostEntry);
        exit(-1);
    }
    
    printf("%s %d\n", hostName, times_connected);

    time_t rawtime;
    struct tm * timeinfo;
    char buf[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timeinfo == NULL) {
        fprintf(stderr, "Error: localtime failed STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
        exit(1);
    }

    strftime(buf, 80, "%a %b %e %H:%M:%S", timeinfo);
    buf[18] = '\n';
    ssize_t write_res = write(connectfd, buf, 19);
    if (write_res == -1) {
        fprintf(stderr, "Error: Write to socket, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
        exit(1);
    }
    if (close(connectfd) == -1) {
        fprintf(stderr, "Error: Closing connection socket, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
        exit(1);
    }
    exit(0);
}


//This function is responsible for starting the server
//Will listen for conections on the port forever.
int start_server(){

    int listenfd, connectfd;
    struct sockaddr_in servAddr, clientAddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        fprintf(stderr, "Error: Socket creation, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
        exit(1);
    }
    // initialize server address structure
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(MY_PORT_NUMBER);
    servAddr.sin_addr.s_addr = htons(INADDR_ANY);

    // bind socket to server address
    if (bind(listenfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1){
        fprintf(stderr, "Error: Binding, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
        close(listenfd);
        exit(1);
    }

    // listen for connections
    if (listen(listenfd, 1) == -1) {
        fprintf(stderr, "Error: Listening, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
        close(listenfd);
        exit(1);
    }
    int times_connected = 0;
    int length = sizeof(clientAddr);

    //forever loop
    while (1) {
        connectfd = accept(listenfd, (struct sockaddr *) &clientAddr, &length);
        times_connected++;
        if (connectfd == -1) {
            fprintf(stderr, "Error: Accepting connection, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
            continue;  // or decide to exit based on the type of error
        }

        int pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Error: Forking failed, STRERR: %s, ERRNO: %d\n", strerror(errno), errno);
            exit(1);
        }
        if (pid == 0) {            // child process
            server_response(connectfd, clientAddr, listenfd, times_connected);  // Call the new function here
        } 
    }
    // should never reach here
    close(listenfd);
    return 0;
}


int main(int argc, char const *argv[]){

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


