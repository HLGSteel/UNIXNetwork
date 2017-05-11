/*
 * Utils.cpp
 *
 *  Created on: May 10, 2017
 *      Author: steel
 */

#include "Utils.h"

#include <config.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include "unp.h"
#include <cstdio>
#include <cstring>

Utils::Utils() {
	// TODO Auto-generated constructor stub

}

Utils::~Utils() {
	// TODO Auto-generated destructor stub
}

/**
 * print the store type of os
 */
void Utils::PrintOSStoreType(){
	union{
			short s;
			char c[sizeof(short)];
		} un;
		un.s = 0x0102;
		printf("%s: ", CPU_VENDOR_OS);
		if(sizeof(short)==2){
			if(un.c[0]==1 && un.c[1]==2)
				printf("big-endian\n");
			else if(un.c[0]==2 && un.c[1]==1)
				printf("little-endian\n");
			else
				printf("inknown\n");
		}else
			printf("sizeof(short)=%d\n",sizeof(short));
}

void Utils::IntroDaytimeTcpsrv1(){
	int listenfd, connfd;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr;
	char buff[MAXLINE];
	time_t ticks;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);

	bind(listenfd, (SA*)&servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	for(;;){
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (SA*)&cliaddr, &len);
		printf("connection from %s, port %d\n",
				inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
				ntohs(cliaddr.sin_port));
		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%s.24s\r\n", ctime(&ticks));
		write(connfd, buff, strlen(buff));

		close(connfd);
	}
}

int Utils::SockfdToFamily(int sockfd){
	struct sockaddr_storage ss;
	socklen_t len;

	len = sizeof(ss);
	if(getsockname(sockfd, (SA*)&ss, &len)<0)
		return -1;
	return ss.ss_family;
}
