/**
*		
*		sftp_client.c
*
*		A very simple file transfer client that uses the socket 
*		library to send a file to a corresponding server
*		over TCP.
*
*		Usage: sftp_client <input_filename> <output_filename> <server_ip_address> <server_port#>
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/** 
	General error handling helper: perror will produce a short error message on the last message encountered during a system or library call, then exit.
*/
void error(char *err) 
{
	perror(err);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd;
	int connfd;
	int port_number;
	
	char filename[256];
	char buffer[10];
	
	struct sockaddr_in server_address;
	struct hostent *server; // Address of server we want to connect to
	
	/* Check # of parameters */
	if( argc < 4 ) {
		fprintf(stderr, "Not enough arguments provided.\nUsage: sftp_client <input_filename> <output_filename> <server_ip_address> <server_port#>\n");
		exit(1);
	}
	
	port_number = atoi(argv[4]);
	
	/* Open socket and store value returned by socket call */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* Socket returns -1 if error opening socket */
	if( sockfd < 0 ) {
		error("Error creating socket");
	}
	
	server = gethostbyname(argv[3]);
	if( server == NULL ) {
		error("Server not found.");
	}
	
	/* Initialize server buffer as 0 */
	memset((char *) &server_address, 0, sizeof(server_address));
	
	server_address.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
	server_address.sin_port = htons(port_number); // network byte order

	/* Attempt to connect socket */
	if( connect(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0 ) {
		error("Error connecting socket");
	}
	
	
	
	FILE *f = fopen(argv[1], "rb" );
	
	if(f == NULL) {
		error("Error opening/reading file");
	}
	
	/* Read file and buffer in chunks of 10 bytes */
	while(1) {
		
		/* Initialize outgoing file buffer */
		memset(buffer, 0, sizeof(buffer));
		
		int bytesRead = fread( buffer, sizeof(char), 10, f );
		printf("Bytes read %d\n", bytesRead);
		
		if(bytesRead > 0){
			// write( sockfd, buffer, strlen(buffer) );
			int length = strlen(buffer);
			int bytes_sent = 0;
			const char *bp = buffer;
			
			while( length > 0 ) {
				
				bytes_sent = send( sockfd, bp, strlen(buffer), 0 );
				
				if( bytes_sent <= 0 ) break;
				if( bytes_sent != 10 ) {
					printf("BYTES SENT OUT: %d !\n", bytes_sent);
				}
				
				bp += bytes_sent;
				length -= bytes_sent;	
					
			}
			
		}
		
		/* File closing conditions */
    if(bytesRead < 10) {
      if(feof(f))
        puts("EOF");
      if(ferror(f))
        puts("There was an error reading from file.");
      break;
    }
	}
	
  return 0;
} // end main