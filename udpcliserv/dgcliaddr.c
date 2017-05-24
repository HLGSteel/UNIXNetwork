#include "unp.h"

void dg_cli(FILE* fp, int sockfd, const SA* pservaddr, socklen_t servlen){
	int n;
	char sendline[MAXLINE], recvline[MAXLINE+1];
	socklen_t len;
	struct sockaddr *preply_addr;
	preply_addr = (struct sockaddr *)malloc(servlen);
	while(fgets(sendline, MAXLINE, fp)!=NULL){
		sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		len = servlen;
		n = recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
		if(len != servlen || memcmp(pservaddr, preply_addr, len)!=0){
			printf("reply from %s (ignored)\n", sock_ntop(preply_addr, len));
			continue;
		}
		recvline[n] = 0;
		fputs(recvline, stdout);
	}
}