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

	static void tcpsvr04();

	static void str_echo(int sockfd);

	static void str_echo08(int sockfd);

	static void tcpcli01(int argc, char** argv);

	static void tcpcli04(int argc, char** argv);

	static void str_cli(FILE* fp, int sockfd);

	static void str_cli11(FILE* fp, int sockfd);

	static void str_cli09(FILE* fp, int sockfd);

	static void str_echo09(int sockfd);

	static void str_cliselect01(FILE* fp, int sockfd);

	static void str_cliselect02(FILE* fp, int sockfd);

	static void tcpservselect01();

	static void tcpservpoll01();
};

#endif /* UTILS_H_ */
