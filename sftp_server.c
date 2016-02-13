/**
*		
*		sftp_server.c
*		
*		A simple file transfer protocol that uses the socket 
*		library to receive and save a file sent by a 
*		client program over TCP.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> // necessary for socket.h
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 1337

/** 
	General error handling helper: perror will produce a short error message on the last message encountered during a system or library call, then exit.
*/
void error(char *err) 
{
	perror(err);
	exit(1);
}

int main(void) 
{

	int sockfd;
	int listenfd;
	uint client_len; // Using int throws int to uint conversion warning
	int file_bytes = 0; // Input file bytes received
	
	char filename[256];
	char buffer[10]; // Write buffer for input file
	
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	
	/* Open socket and store value returned by socket call */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* Socket returns -1 if error opening socket */
	if( sockfd < 0 ) {
		error("Error creating socket");
	}
	
	/* Initialize server buffer as 0 */
	memset((char *) &server_address, 0, sizeof(server_address));
	
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(PORT); // network byte order
	
	/* Attempt to bind socket */
	if( bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0 ) {
		error("Error binding socket");
	}
	
	listen(sockfd, 5); // 5 is number of connections allowed on incoming queue
	puts("Server listening on localhost:1337...");
	
	client_len = sizeof(client_address);
	
	listenfd = accept(sockfd, (struct sockaddr *) &client_address, &client_len);
	if( listenfd < 0 ) {
		error("Error on incoming socket");
	}
	
	/* Initialize incoming file buffer */
	memset(buffer, 0, sizeof(buffer));
	
	/* Create write file */
	FILE *f;
	
	f = fopen("server_file", "wb");
	
	if(f == NULL) {
		error("Error writing to file");
	}
	
	/* recv returns 0 when remote side has closed connection */
	while( (file_bytes = recv(listenfd, buffer, 5, 0)) > 0 ) {
		printf("Bytes received %d\n", file_bytes);
		fwrite(buffer, 1, file_bytes, f);
		memset(buffer, 0, sizeof(buffer)); // zero out buffer
	}
	
	if(file_bytes < 0) {
		puts("File read error. Client connected but file could not be read.");
	}
	
	close(sockfd);
	
	return 0;
} // end main