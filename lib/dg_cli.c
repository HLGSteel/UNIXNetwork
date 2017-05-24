#include "unp.h"

void dg_cli(FILE* fp, int sockfd, const SA *pservaddr, socklen_t servlen){
	int n;
	char sendline[MAX_LINE], recvline[MAXLINE+1];
	while(fgets(sendline, MAX_LINE, fp)!=NULL){
		sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
		recvline[n] = 0;
		fputs(recvline, stdout);
	}
}