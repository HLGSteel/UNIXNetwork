#include "unp.h"
#include "dgcliaddr.c"
#include "../my_err.h"

int main(int argc, char **argv){
	int sockfd;
	struct sockaddr_in servaddr;
	if(argc !=2)
		err_quit("usage: udpcli <IPaddress>");
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	// servaddr.sin_port = htons(7);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	dg_cli(stdin, sockfd, (SA*)&servaddr, sizeof(servaddr));
	exit(0);
}