/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __BOOTSTRAPS_H__
#define __BOOTSTRAPS_H__

#include "baseunit.h"
#include "ChordId.h"
#include "Constants.h"

class BootStrap
{
private :	
	BigInt id ;						//Index
	char address[50];				//ip address
	char port[10];					//port
public :
	BootStrap    *next;
	BootStrap    *parent;
public :
	BootStrap(BigInt , const char * , const char *) ;
	void setid(BigInt x) ;
	void setaddress(const char *addr) ;
	void setport(const char *p) ;
	BigInt getid() ;
	const char *getaddress() ;
	const char *getport() ;
} ;

#endif