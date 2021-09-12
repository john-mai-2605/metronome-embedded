/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <pthread.h>  // for thread nanagement

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

#define SEC_TO_USEC 1000000

// info struct
struct info {
	int sockfd;
	long freq;
};
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

long freq;
void* recv_loop (void* fd) {
	int numbytes;
	char buf[MAXDATASIZE];
	int* sockfd = (int*) fd;
	for (int cc = 0; cc < 10; cc++) {
		if ((numbytes = recv(*sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}

		buf[numbytes] = '\0';

		printf("client: received '%s'\n",buf);
	}
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 3) {
	    fprintf(stderr,"usage: client hostname frequency\n");
	    exit(1);
	}

	freq = strtol(argv[2], NULL, 10);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

    pthread_t tid;
    int err = pthread_create(&tid, NULL, recv_loop, &sockfd);
    if (err != 0) printf("Error in thread creation!");

	for (int cc = 0; cc < 5; cc++) {
		if (send(sockfd, "Client to server (main)!", 25, 0) == -1)
			perror("send");
		usleep(SEC_TO_USEC/freq);
		// else printf("Sent!\n");
	}
    pthread_join(tid, NULL);	
	close(sockfd);

	return 0;
}

