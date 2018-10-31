#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()

// For file fd
#include <fcntl.h>
#include <sys/stat.h>

#define server_ip "127.0.0.1"
#define server_port 9876
#define BUFFER_SIZE 5

int main(){
    int sockfd, addr_len, result;
    struct sockaddr_in address;

    // Create socket for the client
    printf("Creating socket...\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //tcp/ip socket

    if (sockfd < 0) {
        perror("Client ");
        exit(1);
    }

    // Set the server addresses
    printf("Setting up server addresses...\n");
    address.sin_family = AF_INET;
    address.sin_port = htons(server_port);
    address.sin_addr.s_addr = inet_addr(server_ip);
    addr_len = sizeof(address);

    //Connect to the server
    printf("Connecting to server...\n");
    result = connect(sockfd, (struct sockaddr*)&address, addr_len);
    if (result == -1) {
        perror("Client ");
        exit(1);
    }

    printf("Connect sucess!\nEnter file path: ");
    char filepath[1000];
    fgets(filepath, 1000, stdin);

    // Locate the newline character that causing trouble
    char * newline =  strchr(filepath, '\n');

    // Kill it
    if (newline) *newline = '\0';

    // Send the file path to server (the corrent path)

    send(sockfd, filepath, 1000, 0);

    // Recieve the file size from server

    long long filesize; //file can be larger than 2Gb

    recv(sockfd, &filesize, sizeof(filesize), 0);

    if (filesize == -1){
        printf("Something went wrong!\n");
    }
    else {
        printf("The file requested is %lld bytes.\n", filesize);

        // Create buffer to hold the incoming package
        char recvbuffer[BUFFER_SIZE];

        // Create new file pointer
        FILE *fp = fopen(filepath, "a");

        // Clear buffer
        bzero(recvbuffer, BUFFER_SIZE);

        long long byteswrite, bytesrecv, totalbyteswrite = 0;
        bytesrecv = recv(sockfd, recvbuffer, BUFFER_SIZE, 0);
        printf("Recieved %lld bytes\n", bytesrecv);
        while (bytesrecv > 0) {
            byteswrite = fwrite(recvbuffer, sizeof(char), bytesrecv, fp);
            if (byteswrite != bytesrecv) {
                printf("Write failed...\n");
                break;
            }
            totalbyteswrite += byteswrite;
            printf("Write %lld bytes (%.2f %)\n", byteswrite, 100.0*totalbyteswrite/filesize);
            printf(recvbuffer);
            printf("\n");
            bzero(recvbuffer, BUFFER_SIZE);
            bytesrecv = recv(sockfd, recvbuffer, BUFFER_SIZE, 0);
            printf("Recieved %lld bytes\n", bytesrecv);
        }
    }
    // Closing connection
    printf("Closing connection...\n");
    close(sockfd);
    exit(0);
}