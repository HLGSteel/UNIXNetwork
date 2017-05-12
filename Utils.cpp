/*
 * Utils.cpp
 *
 *  Created on: May 10, 2017
 *      Author: steel
 */

#include "Utils.h"

#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <config.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <unp.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>

#include "lib/readline.c"

#include "my_err.h"

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

void Utils::tcpsvr01(){
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port =  htons(SERV_PORT);
	bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);
	for(;;){
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (SA*)&cliaddr, &clilen);
		if((childpid=fork())==0){
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}
}

void Utils::str_echo(int sockfd){
		ssize_t n;
		char buf[MAXLINE];
	again:
		while((n=read(sockfd, buf, MAXLINE))>0)
			write(sockfd, buf, n);
		if(n<0 && errno == EINTR)
			goto again;
		else if(n<0)
			err_sys("str_echo: read error");
	}

void Utils::tcpcli01(int argc, char** argv){
	int sockfd;
	struct sockaddr_in servaddr;
	if(argc!=2)
		err_quit("usage: tcpcli <IPaddress>");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	connect(sockfd, (SA*)&servaddr, sizeof(servaddr));
	str_cli(stdin, sockfd);
	exit(0);
}

void Utils::str_cli(FILE* fp, int sockfd){
	char sendline[MAXLINE], recvline[MAXLINE];
	while(fgets(sendline, MAXLINE, fp)!=NULL){
		write(sockfd, recvline, strlen(sendline));
		if(readline(sockfd, recvline, MAXLINE)==0)
			err_quit("str_cli: server terminated prematurely");
		fputs(recvline, stdout);
	}
}
