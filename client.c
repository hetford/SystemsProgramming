// Lab 8: echo-socket/client.c - simple TCP synchronous sockets - client

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define INPUTSIZ 256

void studentid()
{
	printf("Student ID: \n");
}

void secondoption()
{
	printf("Second option chosen...\n");
}

void thirdoption()
{
	printf("Third option chosen...\n");
}

void displaymenu()
{
	printf("0. Display menu\n");
	printf("1. Return Student ID.\n");
	printf("2. Second option\n");
	printf("3. Third option\n");
	printf("4. Exit\n");
}

int main(void)
{
	int sockfd = 0;
	struct sockaddr_in serv_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Error - could not create socket");
		exit(EXIT_FAILURE);
	}

	serv_addr.sin_family = AF_INET;

	// IP address and port of server we want to connect to - probably best not to hardcode...
	serv_addr.sin_port = htons(50021);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// try to connect...
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
		perror("Error - connect failed");
		exit(EXIT_FAILURE);
	} else
		printf("Connected to server. <Enter with no data to quit>\n");

	unsigned char send_buff[INPUTSIZ];
	unsigned char recv_buff[INPUTSIZ];

	char input[INPUTSIZ];

	// get welcome message
	// be careful with the semantics of read() and write()!
	//  read() will block if no data is available, however if data is available
	//  read() will read *up to* recv_buff bytes and then return if
	//  no further data is currently available; this might happen if:-
	//   - the incoming data has now ended because it was of a smaller size *or*
	//   - further data has been delayed by network buffering
	//  also it's possible there are still more bytes to read than recv_buff bytes
	int bytes_received = read(sockfd, recv_buff, sizeof(recv_buff));
	printf("bytes received: %d\n", bytes_received);
	printf("%s\n", (char *) recv_buff);

	while (1) {
		displaymenu();
		// some console input from user
		printf("option> ");
		fgets(input, INPUTSIZ, stdin);
		input[strcspn(input, "\n")] = 0;


		// send it to server
		strcpy((char *) send_buff, input);
		write(sockfd, send_buff, strlen((char *) send_buff) + 1);	// send it even if its just '\0' for an empty string

		if (strlen(input) == 0)	// empty string...
			break;

		// get some data back - this will block if nothing there yet
		read(sockfd, recv_buff, sizeof(recv_buff));
		printf("%s\n", (char *) recv_buff);
	}

	// always clean up sockets gracefully
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

	exit(EXIT_SUCCESS);
}