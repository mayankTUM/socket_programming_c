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

	//variables for listening dispatcher
	int sockfd_listen_dispatcher, newsockfd_listen_dispatcher, portno_listen_dispatcher;
	struct sockaddr_in low_addr_dispatcher;
	char* hostname_low = "localhost";
	char buffer_recv[256];
	socklen_t clilen;
	struct sockaddr_in cli_addr;

	portno_listen_dispatcher = 50001;

	//listen to dispatcher
	init_listening_params(&sockfd_listen_dispatcher, &low_addr_dispatcher, portno_listen_dispatcher, &clilen, &cli_addr);

	// variables for dispatcher
	int sockfd_high, portno_high, n;
	struct sockaddr_in high_addr;
	struct hostent *high;
	char* hostname_high = "localhost";
	char buffer_high[256];

	portno_high = 50011;


	fd_set fd1;
	FD_ZERO(&fd1);
	FD_SET(sockfd_listen_dispatcher,&fd1);

	while(1) {
		int rc = select(sockfd_listen_dispatcher + 1, &fd1, NULL,NULL,NULL);
		if(rc!=-1) {
			if(FD_ISSET(sockfd_listen_dispatcher,&fd1)) {
				// receive from dispatcher
				newsockfd_listen_dispatcher = accept(sockfd_listen_dispatcher,
						(struct sockaddr *) &cli_addr,
						&clilen);

				bzero(buffer_recv,256);
				n = read(newsockfd_listen_dispatcher,buffer_recv,255);
				if (n < 0)
					error("ERROR reading from socket");
				printf("Execute command: %s",buffer_recv);
				printf("low:%s", buffer_recv);

				// send to HIGH
				init_sending_params(&sockfd_high, &high, hostname_high, &high_addr, portno_high);
				write_data(&sockfd_high, &high_addr, buffer_recv, "low:");
			}
		}
	}


	return 0;
}
