/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820
#include "BootStraps.h"


BootStrap::BootStrap(BigInt x, const char * addr, const char *p) 
{
	setid(x) ;
	setaddress(addr) ;
	setport(p) ;
	next = NULL ;
	parent = NULL ;
}

//------------------------------------------------------------------------

void BootStrap::setid(BigInt x)
{
	id = x ;
}

//------------------------------------------------------------------------

void BootStrap::setaddress(const char *addr) 
{
	int len = strlen(addr) ;
	strncpy(address,addr , len) ;
	strncpy(address+len,"\0",1) ;
}

//------------------------------------------------------------------------

void BootStrap::setport(const char *p) 
{
	int len = strlen(p) ;
	strncpy(port,p , len) ;
	strncpy(port+len,"\0",1) ;
}

//------------------------------------------------------------------------

BigInt BootStrap::getid() 
{
	return id ;
}

//------------------------------------------------------------------------

const char *BootStrap::getaddress() 
{
	return strdup(address) ;
}

//------------------------------------------------------------------------

const char *BootStrap::getport() 
{
	return strdup(port) ;
}

//------------------------------------------------------------------------