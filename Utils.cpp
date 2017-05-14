/*
 * Utils.cpp
 *
 *  Created on: May 10, 2017
 *      Author: steel
 */

#include "Utils.h"

#include <arpa/inet.h>
#include <asm-generic/errno.h>
#include <asm-generic/errno-base.h>
#include <config.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <unp.h>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>

#include "lib/sigchld.c"
#include "lib/readline.c"
#include "lib/sum.h"
#include "my_err.h"

#define OPEN_MAX 1024

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

/**
 * 从客户端读取两个整数求和
 */
void Utils::str_echo08(int sockfd){
	long arg1, arg2;
	ssize_t n;
	char line[MAXLINE];
	for(;;){
		if((n=readline(sockfd, line, MAXLINE))==0) return;
		if(sscanf(line, "%ld%ld", &arg1, &arg2)==2)
			snprintf(line, sizeof(line), "%ld\n", arg1+arg2);
		else
			snprintf(line, sizeof(line), "input error\n");
		n = strlen(line);
		write(sockfd, line, n);
	}
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

void Utils::str_cli11(FILE* fp, int sockfd){
	char sendline[MAXLINE], recvline[MAXLINE];
	while(fgets(sendline, MAXLINE, fp)!=NULL){
		write(sockfd, sendline, 1);
		sleep(1);
		write(sockfd, sendline+1, strlen(sendline)-1);
		if(readline(sockfd, recvline, MAXLINE)==0)
			err_quit("str_cli: server terminated prematurely");
		fputs(recvline, stdout);
	}
}

void Utils::tcpcli04(int argc, char** argv){
	int i, sockfd[5];
	struct sockaddr_in servaddr;
	if(argc !=2) err_quit("usage:tcpcli <IPaddress> ");
	for(i=0;i<5;i++){
		sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port  = htons(SERV_PORT);
		inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
		connect(sockfd[i], (SA*)&servaddr, sizeof(servaddr));
	}
	str_cli(stdin, sockfd[0]);
	exit(0);
}

void Utils::tcpsvr04(){
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	void sig_chld_waitpid(int);
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);
	signal(SIGCHLD, sig_chld_waitpid);
	for(;;){
		clilen = sizeof(cliaddr);
		if((connfd = accept(listenfd, (SA*)&cliaddr, &clilen))<0){
			if(errno == EINTR) continue;
			else err_sys("accept error");
		}
		if((childpid=fork())==0){
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}
}

void Utils::str_cli09(FILE* fp, int sockfd){
	char sendline[MAXLINE];
	struct args args;
	struct result result;
	while(fgets(sendline, MAXLINE, fp)!=NULL){
		if(sscanf(sendline, "%ld%ld", &args.arg1, &args.arg2)!=2){
			printf("invalid input: %s", sendline);
			continue;
		}
		write(sockfd, &args, sizeof(args));
		if(read(sockfd, &result, sizeof(result))==0)
			err_quit("str_cli: server terminated prematurely");
		printf("%ld\n", result.sum);
	}
}

void Utils::str_echo09(int sockfd){
	ssize_t n;
	struct args args;
	struct result result;
	for(;;){
		if((n=read(sockfd, &args, sizeof(args)))==0)
			return ;
		result.sum = args.arg1+args.arg2;
		write(sockfd, &result, sizeof(result));
	}
}

void Utils::str_cliselect01(FILE* fp, int sockfd){
	int maxfdp1;
	fd_set rset;
	char sendline[MAXLINE], recvline[MAXLINE];
	FD_ZERO(&rset);
	for(;;){
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd)+1;
		select(maxfdp1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(sockfd, &rset)){
			if(readline(sockfd, recvline, MAXLINE)==0)
				err_quit("str_cli: server terminated prematurely");
			fputs(recvline, stdout);
		}
		if(FD_ISSET(fileno(fp), &rset)){
			if(fgets(sendline, MAXLINE, fp)==NULL)
				return;
			write(sockfd, sendline, strlen(sendline));
		}
	}
}

void Utils::str_cliselect02(FILE* fp, int sockfd){
	int maxfdp1, stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;
	stdineof = 0;
	FD_ZERO(&rset);
	for(;;){
		if(stdineof==0) FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd)+1;
		select(maxfdp1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(sockfd, &rset)){
			if((n=read(sockfd, buf, MAXLINE))==0){
				if(stdineof==1) return;
				else err_quit("str_cli: server terminated prematurely");
			}
			write(fileno(stdout), buf, n);
		}
		if(FD_ISSET(fileno(fp), &rset)){
			if((n=read(fileno(fp), buf, MAXLINE))==0){
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			write(sockfd, buf, n);
		}
	}
}

void Utils::tcpservselect01(){
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	fd_set rset, allset;
	char buf[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);
	maxfd = listenfd;
	maxi = -1;
	for(i=0;i<FD_SETSIZE;i++) client[i]=-1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for(;;){
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(listenfd, &rset)){
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (SA*)&cliaddr, &clilen);
			for(i=0;i<FD_SETSIZE;i++){
				if(client[i]<0){
					client[i] = connfd;
					break;
				}
			}
			if(i==FD_SETSIZE)
				err_quit("too many clients");
			FD_SET(connfd, &allset);
			if(connfd>maxfd) maxfd = connfd;
			if(i>maxi) maxi=i;
			if(--nready<=0) continue;
		}
		for(i=0;i<=maxi;i++){
			if((sockfd=client[i])<0) continue;
			if(FD_ISSET(sockfd, &rset)){
				if((n=read(sockfd, buf, MAXLINE))==0){
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i]=-1;
				}else
					write(sockfd, buf, n);
				if(--nready<=0) break;
			}
		}
	}
}

void Utils::tcpservpoll01(){
	int i, maxi, listenfd, connfd, sockfd;
	int nready;
	ssize_t n;
	char buf[MAXLINE];
	socklen_t clilen;
	struct pollfd client[OPEN_MAX];
	struct sockaddr_in cliaddr, servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);
	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for(i=1;i<OPEN_MAX;i++)
		client[i].fd = -1;
	maxi =  0;

	for(;;){
		nready = poll(client, maxi+1, INFTIM);
		if(client[0].revents & POLLRDNORM){
			clilen = sizeof(cliaddr);
			connfd  = accept(listenfd, (SA*)&cliaddr, &clilen);
			for(i=1;i<OPEN_MAX;i++){
				if(client[i].fd<0){
					client[i].fd = connfd;
					break;
				}
			}
			if(i == OPEN_MAX)
				err_quit("too many clients");
			client[i].events = POLLRDNORM;
			if(i>maxi) maxi = i;
			if(--nready<=0) continue;
		}
		for(i=1;i<=maxi;i++){
			if((sockfd=client[i].fd)<0) continue;
			if(client[i].revents & (POLLRDNORM | POLLERR)){
				if((n=read(sockfd, buf, MAXLINE))<0){
					if(errno==ECONNRESET){
						close(sockfd);
						client[i].fd = -1;
					}else
						err_sys("read error");
				}else if(n==0){
					close(sockfd);
					client[i].fd = -1;
				}else
					write(sockfd, buf, n);
				if(--nready<=0) break;
			}
		}
	}
}
