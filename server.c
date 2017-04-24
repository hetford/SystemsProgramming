// A TCP synchronous sockets - server

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define INPUTSIZ 256

int main(void)
{
    // socket setup for connection-oriented TCP synchronous full duplex sockets - always the same basically
    // 2 sockets - a listening one and one for the actual connection
    int listenfd = 0, connfd = 0;

    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;	// will contain info about the client when connected
    socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    // listen on all network interfaces (may have > 1 in a server etc.)
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // listen on port 50021
    serv_addr.sin_port = htons(50021);

    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    // all calls to sockets including read() and write() should have their return
    //  values checked like this and errors printed with perror() etc.;
    //  for brevity I have not done this but you should!
    if (listen(listenfd, 10) == -1) {
	perror("Failed to listen");
	exit(EXIT_FAILURE);
    }
    // end socket setup

    // in other words send and receive raw bytes
    unsigned char send_buff[INPUTSIZ];
    unsigned char recv_buff[INPUTSIZ];

    // now actually listen - accept() blocks until connection accepted
    // client_addr will then hold info about connected client
    // notice this returns a new socket on a new (random) port
    printf("Waiting for a client to connect...\n");
    connfd = accept(listenfd, (struct sockaddr *) &client_addr, &socksize);

    // display IP address of connected client
    printf
	("Accepted connection from client at: %s on new local port: %d\n",
	 inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // send welcome message to the client
    strcpy((char *) send_buff, "Welcome from server...");
    int bytes_sent = write(connfd, send_buff, strlen((char *) send_buff) + 1);
    printf("bytes sent: %d\n", bytes_sent); 

    while (1) {
	// get some data from client - this will block if no data
        //  see other comments in client.c
	read(connfd, recv_buff, sizeof(recv_buff));
	if (strlen((char *) recv_buff) == 0)	// break if received '\0' i.e. an empty string
	    break;
	printf("Received: %s\n", (char *) recv_buff);

	// make the data look slightly different for demo purposes
	strcpy((char *) send_buff, "From server: ");
	strcat((char *) send_buff, (char *) recv_buff);

	// send the data back to the client
	write(connfd, send_buff, strlen((char *) send_buff) + 1);
    }



    // always clean up sockets gracefully
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    // shutdown not required on listening sockets
    close(listenfd);

    exit(EXIT_SUCCESS);
}
