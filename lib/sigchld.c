/*
 * sigchld.c
 *
 *  Created on: May 14, 2017
 *      Author: steel
 */

#include "unp.h"

/**
 * wait版本的sigchild处理函数
 */
void sig_chld_wait(int signo){
	pid_t pid;
	int stat;

	pid = wait(&stat);
	printf("child %d terminated\n", pid);
	return;
}

/**
 * waitpid版本的sigchld函数，可避免出现僵死进程
 */
void sig_chld_waitpid(int signo){
	pid_t pid;
	int stat;

	while((pid=waitpid(-1, &stat, WNOHANG))>0)
		printf("child %d terminated\n", pid);
	return;
}
