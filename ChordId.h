/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __CHORDID_H__
#define __CHORDID_H__

#include "baseunit.h"

class ChordId
{
private :
	BigInt id ;						//Index
	char address[50];				//ipaddress
	char port[10];					//port
	void *constants ;				//constants
public:
	//ChordId() ;
	ChordId(BigInt i = -1 ,void *cons = NULL ,const char * a =NULL ,const char * p =NULL) ;
	ChordId(char *a , char *p , void *cons) ;
	unsigned int shash(char* str) ; 
	BigInt GetId() ;
	char * GetId_str() ;
	void SetId(int i) ;
	char * GetAddress() ;
	void SetAddress(const char * addr) ;
	char * GetPort() ;
	void SetPort(const char * p) ;

	/*if id=0£¬return true*/
	bool IsEmptyChordId() ;
	bool BelongsRightInclusive(ChordId first , ChordId second) ;
	bool belongs(ChordId first, ChordId second) ;
	bool isLower(ChordId anotherId) ;
	bool isLowerEqual(ChordId anotherId) ;
	bool isGreater(ChordId anotherId)  ;
	bool isGreaterEqual(ChordId anotherId) ;
	ChordId add(ChordId bId) ;
	ChordId subtract(ChordId bId) ;
	int msb() ;
	int compareTo(ChordId obj) ;
	bool equals(ChordId obj) ;
	
	bool EqualsWithAddr(ChordId obj) ;
	char *toString() ;
} ;



#endif