#include "unp.h"

#define NDG 2000
#define DGLEN 1400

void dg_cli(FILE* fp, int sockfd, const SA* pservaddr, socklen_t servlen){
	int i;
	char sendline[DGLEN];
	for(i=0;i<NDG;i++){
		sendto(sockfd, sendline, DGLEN, 0, pservaddr, servlen);
	}
}