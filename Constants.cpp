/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820
#include "constants.h"
#include <math.h>

Constants::Constants(PreferencesCore *prefer)
{
	preferencesCore = prefer ;
	//two = BigInteger.ONE.add(BigInteger.ONE);
	//init();
	nbits = 5;
    idlen = nbits / 8;
    nfingers = nbits;
    nsuccessors = 3;
    bufsize = 65536;
    stabilize_period = 600000000;
    max_wellknown = 4;
    ping_thresh = 3 ;
    ChordSpace = (int)pow(2,nbits) ;
	MaxChordId = ChordId(ChordSpace,this,"","");
    MinChordId = ChordId(0,this,"","");
}
//--------------------------------------------------------------------------
void Constants::init() 
{
	/*从配置中获取*/
	/*
    nbits = PreferencesCore->getIntPref("chord.nbits");
    idlen = nbits / 8;
    nfingers = nbits;
    nsuccessors = PreferencesCore->getIntPref("chord.nsuccessors");
    bufsize = PreferencesCore->getIntPref("chord.bufsize");
    stabilize_period = PreferencesCore->getIntPref("chord.stabilize_period");
    max_wellknown = PreferencesCore->getIntPref("chord.max_wellknown");
    ping_thresh = PreferencesCore->getIntPref("chord.ping_thresh");
    chordSpace = two.pow(nbits);
    maxChordId = new ChordId(two.pow(nbits).subtract(BigInteger.ONE),this);
    minChordId = new ChordId(BigInteger.ZERO,this);
	*/
}
//--------------------------------------------------------------------------
int Constants::getNbits()
{
    return nbits;
}
//--------------------------------------------------------------------------
int Constants::getIdlen()
{
	return idlen;
}
//--------------------------------------------------------------------------
int Constants::getNfingers() 
{
    return nfingers;
}
//--------------------------------------------------------------------------
int Constants::getNsuccessors() 
{
    return nsuccessors;
}
//--------------------------------------------------------------------------
int Constants::getBufsize() 
{
    return bufsize;
}
//--------------------------------------------------------------------------
int Constants::getStabilize_period() 
{
    return stabilize_period;
}
//--------------------------------------------------------------------------
int Constants::getMax_wellknown() 
{
    return max_wellknown;
}
//--------------------------------------------------------------------------
BigInt Constants::getChordSpace() 
{
    return ChordSpace;
}
//--------------------------------------------------------------------------