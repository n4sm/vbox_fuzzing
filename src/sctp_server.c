// SCTP Server (server.c)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

#define SERVER_PORT 9877
#define MAX_BUFFER 1024
#define MAX_STREAM 5

int main() {
    int listen_fd, conn_fd;
    struct sockaddr_in servaddr;
    char buffer[MAX_BUFFER];
    struct sctp_initmsg initmsg;
    struct sctp_sndrcvinfo sndrcvinfo;
    int flags;

    // Create an SCTP TCP-style socket
    listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (listen_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Initialize server address (IPv4)
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    // Bind the socket to the server address
    if (bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(listen_fd);
        exit(1);
    }

    // Set SCTP Init parameters
    bzero(&initmsg, sizeof(initmsg));
    initmsg.sinit_num_ostreams = MAX_STREAM;
    initmsg.sinit_max_instreams = MAX_STREAM;
    initmsg.sinit_max_attempts = 4;
    setsockopt(listen_fd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));

    // Listen for incoming connections
    if (listen(listen_fd, 5) < 0) {
        perror("Listen failed");
        close(listen_fd);
        exit(1);
    }

    printf("SCTP server is running on port %d...\n", SERVER_PORT);

    // Accept client connections
    conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
    if (conn_fd < 0) {
        perror("Accept failed");
        close(listen_fd);
        exit(1);
    }

    // Receive messages from the client
    while (1) {
        bzero(buffer, MAX_BUFFER);
        flags = 0;
        int recv_len = sctp_recvmsg(conn_fd, buffer, sizeof(buffer), NULL, 0, &sndrcvinfo, &flags);
        if (recv_len > 0) {
            printf("Received from client: %s\n", buffer);
            // Echo the message back to the client
            sctp_sendmsg(conn_fd, buffer, strlen(buffer), NULL, 0, 0, 0, sndrcvinfo.sinfo_stream, 0, 0);
        } else {
            printf("Connection closed by client\n");
            break;
        }
    }

    close(conn_fd);
    close(listen_fd);
    return 0;
}
