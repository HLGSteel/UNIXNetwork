/*
 * Utils.h
 *
 *  Created on: May 10, 2017
 *      Author: steel
 */

#ifndef UTILS_H_
#define UTILS_H_

class Utils {
public:
	Utils();
	virtual ~Utils();

	static void PrintOSStoreType();

	static void IntroDaytimeTcpsrv1();

	static int SockfdToFamily(int sockfd);
};

#endif /* UTILS_H_ */
