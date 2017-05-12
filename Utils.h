/*
 * Utils.h
 *
 *  Created on: May 10, 2017
 *      Author: steel
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <cstdio>

class Utils {
public:
	Utils();
	virtual ~Utils();

	static void PrintOSStoreType();

	static void IntroDaytimeTcpsrv1();

	static int SockfdToFamily(int sockfd);

	static void tcpsvr01();

	static void str_echo(int sockfd);

	static void tcpcli01(int argc, char** argv);

	static void str_cli(FILE* fp, int sockfd);
};

#endif /* UTILS_H_ */
