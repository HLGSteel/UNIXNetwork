#include "unp.h"

static void recvfrom_int(int);
static int count;

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen){
	socklen_t len;
	char mesg[MAXLINE];
	signal(SIGINT, recvfrom_int);
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