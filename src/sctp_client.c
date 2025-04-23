// SCTP Client (client.c)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

#define SERVER_PORT 9877
#define MAX_BUFFER 1024

int main() {
    int sock_fd;
    struct sockaddr_in servaddr;
    char buffer[MAX_BUFFER];

    // Create an SCTP TCP-style socket
    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Initialize server address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(SERVER_PORT);

    // Connect to the SCTP server
    if (connect(sock_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection failed");
        close(sock_fd);
        exit(1);
    }

    // write(sock_fd, "test123", strlen("test123"));

    // // Send messages to the server
    // while (1) {
    //     printf("Enter message to send: ");
    //     fgets(buffer, MAX_BUFFER, stdin);
    //     buffer[strlen(buffer) - 1] = '\0';  // Remove newline

    //     // Send message to the server
    //     sctp_sendmsg(sock_fd, buffer, strlen(buffer), NULL, 0, 0, 0, 0, 0, 0);

    //     // Exit loop if "exit" is entered
    //     if (strcmp(buffer, "exit") == 0) {
    //         break;
    //     }

    //     // Receive echo from the server
    //     bzero(buffer, MAX_BUFFER);
    //     int recv_len = sctp_recvmsg(sock_fd, buffer, sizeof(buffer), NULL, 0, NULL, NULL);
    //     if (recv_len > 0) {
    //         printf("Received from server: %s\n", buffer);
    //     }
    // }

    close(sock_fd);
    return 0;
}