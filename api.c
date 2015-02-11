#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

void init_listening_params(int* sockfd, struct sockaddr_in* addr, int portno, socklen_t* clilen, struct sockaddr_in* cli_addr) {
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(*sockfd, F_SETFL, O_NONBLOCK);
	if (*sockfd < 0)
		error("ERROR opening own socket");
	bzero((char *) addr, sizeof(*addr));


	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(portno);
	if (bind(*sockfd, (struct sockaddr *) addr,
			sizeof(*addr)) < 0)
		error("ERROR on binding own port");

	listen(*sockfd,5);
	*clilen = sizeof(*cli_addr);
}

void init_sending_params(int* sockfd, struct hostent **receiver, char* receiver_ip, struct sockaddr_in* receiver_addr, int portno_receiver) {
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (*sockfd < 0)
		error("ERROR opening receiver socket");
	*receiver = gethostbyname(receiver_ip);
	if (*receiver == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) receiver_addr, sizeof(*receiver_addr));
	receiver_addr->sin_family = AF_INET;
	bcopy((char *)(*receiver)->h_addr,
			(char *)&receiver_addr->sin_addr.s_addr,
			(*receiver)->h_length);
	receiver_addr->sin_port = htons(portno_receiver);
}

void write_data(int* sockfd, struct sockaddr_in* addr, char data[], char* prefix) {
	int n;
	if (connect(*sockfd,(struct sockaddr *) addr,sizeof(*addr)) < 0) {
		printf("Receiver not yet ready \n");
		exit(1);
	}
	else {
		char append[256];
		strcpy(append,prefix);
		strcat(append, data);
		n = write(*sockfd,append,strlen(append));
		if (n < 0)
			error("ERROR writing to socket");
	}

}
