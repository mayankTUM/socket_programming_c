/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

int max (int a,int b) {
	return a>b ? a : b;
}

int main(int argc, char *argv[]) {

	// variables for user
	int sockfd_user, portno_user, n;
	struct sockaddr_in user_addr;
	struct hostent *user;
	char* hostname_user = "localhost";
	char buffer_user[256];

	//sending port for user
	portno_user = 50030;

	//client code for user


	// ----------------------------------------------------------------------------------------

	//variables for high
	int sockfd_high, portno_high;
	struct sockaddr_in high_addr;
	struct hostent *high;
	char* hostname_high = "localhost";
	char buffer_high[256];

	//sending port for high
	portno_high = 50007;

	//client code for high



	// ----------------------------------------------------------------------------------------

	//variables for low

	int sockfd_low, portno_low;
	struct sockaddr_in low_addr;
	struct hostent *low;
	char* hostname_low = "localhost";
	char buffer_low[256];

	//sending port for low

	portno_low = 50001;




	// -------------------------------------------------------------------------------------------------


	//variables for self
	int sockfd_listen_user, newsockfd_listen_user, portno_listen_user;
	int sockfd_listen_high, newsockfd_listen_high, portno_listen_high;

	struct sockaddr_in dispatch_addr_user;
	struct sockaddr_in dispatch_addr_high;
	char* hostname_dispatch = "localhost";
	char buffer_recv_user[256];
	char buffer_recv_high[256];
	socklen_t clilen_dispatch_user;
	socklen_t clilen_dispatch_high;
	struct sockaddr_in cli_addr_user;
	struct sockaddr_in cli_addr_high;

	//lisening ports
	portno_listen_user = 50022;
	portno_listen_high = 50021;

	//listen to user
	init_listening_params(&sockfd_listen_user, &dispatch_addr_user, portno_listen_user, &clilen_dispatch_user, &cli_addr_user);


	//listen to high
	init_listening_params(&sockfd_listen_high, &dispatch_addr_high, portno_listen_high, &clilen_dispatch_high, &cli_addr_high);

	char* mode = "high";

	while(1) {
		//do something
		fd_set fd1;
		FD_ZERO(&fd1);
		//	FD_ZERO(&fd2);
		FD_SET(sockfd_listen_user,&fd1);
		FD_SET(sockfd_listen_high,&fd1);
		//
		int rc = select(max(sockfd_listen_user, sockfd_listen_high)+1, &fd1, NULL,NULL,NULL);
		if(rc!=-1) {
			if(FD_ISSET(sockfd_listen_user,&fd1)) {
				bzero(buffer_recv_user,256);
				newsockfd_listen_user = accept(sockfd_listen_user,
						(struct sockaddr *) &cli_addr_user,
						&clilen_dispatch_user);
				n = read(newsockfd_listen_user,buffer_recv_user,255);
				char *e;
				int index;
				e = strchr(buffer_recv_user, '\n');
				index = (int)(e - buffer_recv_user);

				if (n < 0)
					error("ERROR reading from user");
				if(strncmp(buffer_recv_user, "switch_to_high", index-1) == 0) {
					mode = "high";
					printf("switch to high security mode\n");
				} else if(strncmp(buffer_recv_user, "switch_to_low", index-1) == 0) {
					mode = "low";
					printf("switch to low security mode\n");
				} else {
					printf("forwarding command %s", buffer_recv_user);
					if(mode == "high") {
						// send to high - client code
						init_sending_params(&sockfd_high, &high, hostname_high, &high_addr, portno_high);
						write_data(&sockfd_high, &high_addr, buffer_recv_user,"");

					} else {
						// send to low - client code
						init_sending_params(&sockfd_low, &low, hostname_low, &low_addr, portno_low);
						write_data(&sockfd_low, &low_addr, buffer_recv_user,"");
					}
				}
			}
			else if(FD_ISSET(sockfd_listen_high,&fd1)) {

				newsockfd_listen_high = accept(sockfd_listen_high,
						(struct sockaddr *) &cli_addr_high,
						&clilen_dispatch_high);

				bzero(buffer_recv_high,256);
				n = read(newsockfd_listen_high,buffer_recv_high,255);
				if (n < 0)
					error("ERROR reading from high");

				//forward it to user
				init_sending_params(&sockfd_user, &user, hostname_user, &user_addr, portno_user);
				printf("Forwarding result : %s", buffer_recv_high);
				write_data(&sockfd_user, &user_addr, buffer_recv_high,"");

			}
			else {
				printf("received messages from an unknown socket\n");
			}

		}

	}

	close(sockfd_listen_user);
	close(sockfd_listen_high);
	close(sockfd_user);
	close(sockfd_high);
	return 0;
}


