/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0826
#include <stdlib.h>
#include "ChordId.h"
#include "constants.h"
#include "baseunit.h"

ChordId::ChordId(BigInt i ,void *cons ,const char * a ,const char * p) 
{
	id = i ;
	if(a)
		strcpy(address,a) ;
	if(p)
		strcpy(port,p) ;

	constants = cons ;
}
//-----------------------------------------------------------------
ChordId::ChordId(char * a , char * p , void *cons)
{
	strcpy(address,a) ;
	strcpy(port,p) ;
	char *tmp = new char[50] ;
	strcpy(tmp,a) ;
	strcat(tmp,":");
	strcat(tmp,p) ;
	id = (BigInt)shash((char *)a) ;
	delete tmp ;
	constants = cons ;
}
//-----------------------------------------------------------------
unsigned int ChordId::shash(char* str)
{
	/*unsigned int hash = 0;
	unsigned int x    = 0;

    for(char *p = str; p < str + strlen(str); p++)
    {
		hash = (hash << 4) + *p;
        if((x = hash & 0xF0000000L) != 0)
        {
                hash ^= (x >> 24);
                hash &= ~x;
        }
    }

    return (hash & 0x1F);*/
	char *tmp =  str ;
	tmp = strchr(str,'.');
	tmp = strchr(tmp+1,'.');
	tmp = strchr(tmp+1,'.');
	unsigned int k = atoi(tmp + 1) + atoi(port) ;
	return (k%32) ;
}
//-----------------------------------------------------------------
BigInt ChordId::GetId() 
{
	return id ;
}
//-----------------------------------------------------------------
char *ChordId::GetId_str() 
{
	char idstr[40]  ;
	_snprintf(idstr , sizeof(idstr) , "%d" , id) ;
	return strdup(idstr) ;
}
//-----------------------------------------------------------------
void ChordId::SetId(int i) 
{
	id = i ;
}
//-----------------------------------------------------------------
char * ChordId::GetAddress() 
{
    return address;
}
//-------------------------------------------------------------------
void ChordId::SetAddress(const char * addr) 
{
	strncpy(address,addr,49) ;
}
//-------------------------------------------------------------------
char * ChordId::GetPort() 
{
	return port;
}
//-------------------------------------------------------------------
void ChordId::SetPort(const char * p) 
{
	strncpy(port,p,9) ;   
}
//-------------------------------------------------------------------
bool ChordId::IsEmptyChordId() 
{
	if(id == -1)
		return true ;
	else
		return false ;
}
//-------------------------------------------------------------------
bool ChordId::BelongsRightInclusive(ChordId first , ChordId second) 
{
	if (first.equals(second))
	{
		return true ;
	}
    if(first.isLower(second)) {
        if (isLowerEqual(second) && isGreater(first))
            return true;
        else
            return false;
    }
    else 
	{
        if (isLowerEqual(((Constants *)constants)->MaxChordId) && isGreater(first))
            return true;
        else if (isLowerEqual(second) && isGreaterEqual(((Constants *)constants)->MinChordId))
            return true;
        else
            return false;
    }
}
//-----------------------------------------------------------------
bool ChordId::belongs(ChordId first, ChordId second) 
{
	if (first.equals(second))
	{
		if(!equals(first))
			return true ;
		else
			return false;// n is the only node not in the interval (n,n)
	}
    if(first.isLower(second)) {
        if (isLower(second) && isGreater(first))
            return true;
        else
            return false;
    }
    else {
        if (isLowerEqual(((Constants *)constants)->MaxChordId) && isGreater(first))
            return true;
        else if (isLower(second) && isGreaterEqual(((Constants *)constants)->MinChordId))
            return true;
        else
            return false;
    }
}
//-----------------------------------------------------------------
bool ChordId::isLower(ChordId anotherId) 
{
	return compareTo(anotherId) < 0;
}
//-----------------------------------------------------------------
bool ChordId::isLowerEqual(ChordId anotherId) 
{
	return compareTo(anotherId) <= 0;
}
//-----------------------------------------------------------------
bool ChordId::isGreater(ChordId anotherId)  
{
	return compareTo(anotherId) > 0;
}
//-----------------------------------------------------------------
bool ChordId::isGreaterEqual(ChordId anotherId) 
{
	return compareTo(anotherId) >= 0;
}
//-----------------------------------------------------------------
ChordId ChordId::add(ChordId bId) 
{
	id = id + bId.GetId();
    id = id % ((Constants *)constants)->getChordSpace();
    return ChordId(id, constants,"","");
}
//-----------------------------------------------------------------
ChordId ChordId::subtract(ChordId bId) 
{
	id = id - bId.GetId();
	id = id % ((Constants *)constants)->getChordSpace();
	return ChordId(id,constants,"", "");
}
//-----------------------------------------------------------------
int ChordId::msb() 
{
	/*int i;
    byte[] x = id.toByteArray();
    for (i = 0; i < constants.GetIdlen(); i++)
        if (x[i] != 0)
            return 8 * i + msb_tab[x[i]];
    return 0;*/
	return 0 ;
}
//-----------------------------------------------------------------
int ChordId::compareTo(ChordId obj) 
{
	if (id > obj.GetId())
		return 1 ;
	else if(id < obj.GetId())
		return -1 ;
	else
		return 0 ;
}
//-----------------------------------------------------------------
bool ChordId::equals(ChordId obj) 
{
	return compareTo(obj) == 0;
}
//-----------------------------------------------------------------
bool ChordId::EqualsWithAddr(ChordId obj) 
{
	if(equals(obj) && obj.port == port && obj.address == address)
		return true ;
	else
		return false ;
}
//-----------------------------------------------------------------
char *ChordId::toString() 
{
	return 0 ;
}