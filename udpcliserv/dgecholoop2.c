#include "unp.h"

static void recvfrom_int(int);
static int count;

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen){
	int n;
	socklen_t len;
	char mesg[MAXLINE];
	signal(SIGINT, recvfrom_int);
	n = 220*1024;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));
	for(;;){
		len = clilen;
		recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
		count++;
	}
}

static void recvfrom_int(int signo){
	printf("\nreceived %d datagrams\n", count);
	exit(0);
}