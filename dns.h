#include <iostream>
#include <string.h>
#include <bitset>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>

struct HEADER{

	/*Identification*/
	unsigned short ID;				//Id number - 16 bit
	
	/*Information*/
	unsigned char RD :1; 			//RD - 1 bit
	unsigned char TC :1;				//TC - 1 bit
	unsigned char AA :1;				//AA - 1 bit
	unsigned char OpCode :4;		//OpCode - 4 bit
	unsigned char QR :1;				//QR - 1 bit
	unsigned char RCODE :4;			//RCODE - 4 bit
	unsigned char Z :3;				//Z - 3 bit
	unsigned char RA :1;				//RA - 1 bit
	
	/*Question and Answer*/
	unsigned short Q_COUNT;			//number of questions - 16 bit
	unsigned short A_COUNT;			//number of answers - 16 bit
	unsigned short N_COUNT;			//number of server records - 16 bit (disregard)
	unsigned short AR_COUNT;		//number of additional records - 16 bit (disregard)
};

struct QUESTION{
	unsigned short Q_TYPE;			//16 bit
	unsigned short Q_CLASS;			//16 bit
};



