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

	//variables for dispatcher
	int sockfd_dispatcher, portno_dispatcher,n;
	struct sockaddr_in dispatcher_addr;
	struct hostent *dispatcher;
	char* hostname_dispatcher = "localhost";
	char buffer_dispatcher[256];

	//sending port for high
	portno_dispatcher = 50021;

	//client code for high


	//variables for self
	int sockfd_listen_low, newsockfd_listen_low, portno_listen_low;
	int sockfd_listen_dispatcher, newsockfd_listen_dispatcher, portno_listen_dispatcher;

	struct sockaddr_in high_addr_low;
	struct sockaddr_in high_addr_dispatch;
	char* hostname_high = "localhost";
	char buffer_recv_low[256];
	char buffer_recv_dispatch[256];
	socklen_t clilen_high_low;
	socklen_t clilen_high_dispatch;
	struct sockaddr_in cli_addr_low;
	struct sockaddr_in cli_addr_dispatch;

	//lisening ports
	portno_listen_low = 50011;
	portno_listen_dispatcher = 50007;

	//listen to dispatcher
	init_listening_params(&sockfd_listen_dispatcher, &high_addr_dispatch, portno_listen_dispatcher, &clilen_high_dispatch, &cli_addr_dispatch);


	//listen to low
	init_listening_params(&sockfd_listen_low, &high_addr_low, portno_listen_low, &clilen_high_low, &cli_addr_low);

	while(1) {
		fd_set fd1;
		FD_ZERO(&fd1);
		FD_SET(sockfd_listen_low,&fd1);
		FD_SET(sockfd_listen_dispatcher,&fd1);

		int rc = select(max(sockfd_listen_low, sockfd_listen_dispatcher)+1, &fd1, NULL,NULL,NULL);
		if(rc!=-1) {
			if(FD_ISSET(sockfd_listen_low,&fd1)) {
				//do something
				// receive from low
				newsockfd_listen_low = accept(sockfd_listen_low,
						(struct sockaddr *) &cli_addr_low,
						&clilen_high_low);

				bzero(buffer_recv_low,256);
				n = read(newsockfd_listen_low,buffer_recv_low,255);
				if (n < 0)
					error("ERROR reading from low");

				// forward it to dispatcher
				init_sending_params(&sockfd_dispatcher, &dispatcher, hostname_dispatcher, &dispatcher_addr, portno_dispatcher);
				printf("Forwarding result %s", buffer_recv_low);
				write_data(&sockfd_dispatcher, &dispatcher_addr, buffer_recv_low,"");
			}
			else if(FD_ISSET(sockfd_listen_dispatcher,&fd1)) {
				//do something

				// receive from dispatcher

				newsockfd_listen_dispatcher = accept(sockfd_listen_dispatcher,
						(struct sockaddr *) &cli_addr_dispatch,
						&clilen_high_dispatch);


				bzero(buffer_recv_dispatch,256);
				n = read(newsockfd_listen_dispatcher,buffer_recv_dispatch,255);
				if (n < 0)
					error("ERROR reading from dispatcher");
				// execute

				printf("Executing command: %s",buffer_recv_dispatch);
				printf("high:%s", buffer_recv_dispatch);

				// revert to dispatcher
				init_sending_params(&sockfd_dispatcher, &dispatcher, hostname_dispatcher, &dispatcher_addr, portno_dispatcher);
				write_data(&sockfd_dispatcher, &dispatcher_addr, buffer_recv_dispatch, "high:");

			}
		}
	}
	close(sockfd_listen_dispatcher);
	close(sockfd_listen_low);
	close(sockfd_dispatcher);
	return 0;
}
