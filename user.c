/*
 *  User for the starlight example. Listen to both input
 *  on command line and incoming message.
 *  On input from command line, sends its content to the dispatch.
 *  On reception of a message displays it to the user.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>


int max (int a,int b) {
	return a>b ? a : b;
}

int main(int argc, char *argv[])
{
	// variables for dispatcher
	int sockfd_dispatch, portno_dispatch, n;
	struct sockaddr_in dispatch_addr;
	struct hostent *dispatch;
	char* hostname_dispatch = "localhost";
	char buffer_stdin[256];


	//portno = atoi(argv[2]);
	portno_dispatch = 50022;



	//variables for self
	int sockfd_user, newsockfd_user, portno_user;
	struct sockaddr_in user_addr;
	char* hostname_user = "localhost";
	char buffer_recv[256];
	socklen_t clilen;
	struct sockaddr_in cli_addr;
	portno_user = 50030;


	init_listening_params(&sockfd_user,&user_addr,portno_user,&clilen,&cli_addr);



	while(1) {
		fd_set fd1;
		FD_ZERO(&fd1);
		FD_SET(STDIN_FILENO,&fd1);
		FD_SET(sockfd_user,&fd1);

		printf("cmd: ");
		fflush(stdout);
		//place to insert select system call
		int rc = select(max(STDIN_FILENO, sockfd_user) + 1, &fd1, NULL,NULL,NULL);
		if(rc!=-1) {
			if(FD_ISSET(STDIN_FILENO,&fd1)) {

				// client code - sending code
				init_sending_params(&sockfd_dispatch, &dispatch, hostname_dispatch, &dispatch_addr, portno_dispatch);

				bzero(buffer_stdin,256);
				fgets(buffer_stdin,255,stdin);

				write_data(&sockfd_dispatch, &dispatch_addr, buffer_stdin,"");

			}
			else if(FD_ISSET(sockfd_user,&fd1)) {
				newsockfd_user = accept(sockfd_user,
						(struct sockaddr *) &cli_addr,
						&clilen);

				bzero(buffer_recv,256);
				n = read(newsockfd_user,buffer_recv,255);
				if (n < 0)
					error("ERROR reading from socket");
				printf("received: %s",buffer_recv);
			}
		}
	}
	close(sockfd_dispatch);
	close(sockfd_user);
	return 0;
}
