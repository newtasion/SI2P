/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__



#include "baseunit.h"
#include "PreferencesCore.h"
#include "ChordId.h"

/*for temp*/
#define NBITS 5
#define NSUCCLIST 3


class Constants
{
private :
	PreferencesCore *preferencesCore ; 
	int nbits ;
	int idlen ;
	int nfingers ;
	int nsuccessors ;
	int bufsize ;
	int stabilize_period ;
	int max_wellknown ;
	int ping_thresh ;
	BigInt two ;
	//int FIND_SUCCESSOR = 1 ;
	BigInt ChordSpace ;

public:
	ChordId MaxChordId ;
	ChordId MinChordId ;
	Constants(PreferencesCore *prefer) ;
	void init() ;
	int getNbits() ;
	int getIdlen() ;
	int getNfingers() ;
	int getNsuccessors() ;
	int getBufsize() ;
	int getStabilize_period() ;
	int getMax_wellknown() ;
	BigInt getChordSpace() ;
} ;

#endif