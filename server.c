#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

// Header for file stat, fd
#include <sys/stat.h>
#include <fcntl.h>

// bzero
#include <strings.h>

#define server_ip "127.0.0.1"
#define server_port 9876
#define BUFFER_SIZE 5

int main(){
    int server_sockfd, client_sockfd, server_addrlen, client_addrlen;
    struct sockaddr_in server_address, client_address;
    int result;

    //Create a unnamed socket for server
    printf("Creating socket...\n");
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0); //tcp/ip

    if (server_sockfd < 0){
        perror("Server ");
        exit(1);
    }
    // Setup server address
    printf("Setting ip server address...\n");
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_addrlen = sizeof(server_address);

    // Bind the socket to the network interface
    printf("Binding...\n");
    result = bind(server_sockfd, (struct sockaddr*)&server_address, server_addrlen);

    if (result == -1){
        perror("Server ");
        exit(1);
    }

    // Create listen queue
    printf("Creating listen queue...\n");
    listen(server_sockfd, 5);

    char filepath[1000];

    while(1){
        // Accepting connection
        printf("Waiting for connections...\n");
        client_addrlen = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address, &client_addrlen);

        // Read incoming file path request
        recv(client_sockfd, filepath, 1000, 0);
        printf("Recieved: %s\n", filepath);

        // Get the stat of the file requested

        struct stat filestat;
        long long filesize; //file size might > 2Gb
        
        if ( stat(filepath, &filestat) != 0){
            perror("File ");
            filesize = -1;
        }
        else {
            filesize = filestat.st_size;
        }

        // Send the file size back to client
        send(client_sockfd, &filesize, sizeof(filesize), 0);

        // If the file exist, send it to client
        if (filesize != -1){
            char sendbuffer[BUFFER_SIZE];

            // Create file pointer to the file
            FILE *fp =  fopen(filepath, "r");

            // Clear the buffer
            bzero(sendbuffer, BUFFER_SIZE);


            long long bytesread, bytessend, totalbytessend = 0;
            bytesread = fread(sendbuffer, sizeof(char), BUFFER_SIZE, fp);
            printf("Read %lld bytes\n", bytesread);
            while (bytesread > 0){
                bytessend = send(client_sockfd, sendbuffer, BUFFER_SIZE, 0);
                if (bytessend < 0){
                    printf("Sent failed...\n");
                    break;
                }
                totalbytessend += bytessend;
                printf("Send %lld bytes\n", bytessend);
                bzero(sendbuffer, BUFFER_SIZE);
                bytesread = fread(sendbuffer, sizeof(char), BUFFER_SIZE, fp);
                printf("Read %lld bytes\n", bytesread);
            }   
        }
        close(client_sockfd);
    }

    exit(0);
}