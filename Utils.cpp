/*
 * Utils.cpp
 *
 *  Created on: May 10, 2017
 *      Author: steel
 */

#include "Utils.h"
#include "unp.h"
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
