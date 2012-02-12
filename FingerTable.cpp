/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include "constants.h"
#include "FingerTable.h"
#include "Node.h"
#include <math.h>

//-----------------------------------------------------------------------

FingerTable::FingerTable(void *cons, Node * node) 
{
	constants = cons ;
	localnode = node ;
	initfingers();
}

//-----------------------------------------------------------------------

FingerTable::~FingerTable() 
{
	constants = NULL ;
	localnode = NULL ;
}

//-----------------------------------------------------------------------

void FingerTable::initfingers() 
{
	ChordId EmptyId ;
	for (int pos = 1; pos <= getTableLength(); pos++) 
	{
        /*generater start*/
		ChordId start = ChordId(localnode->getChordId().GetId(),constants,"","");
		start.add(ChordId(int(pow(2,pos-1)),constants,"",""));

		/*generater end*/
		ChordId end = ChordId(localnode->getChordId().GetId(),constants,"","");
		if(pos != NBITS) 
			end.add(ChordId(int(pow(2,pos)),constants,"",""));
    
		fingers[pos-1].start = start ;
		fingers[pos-1].interval[0] = start ;
		fingers[pos-1].interval[1] = end ;

		//init the all nodes
		fingers[pos-1].node = EmptyId ;
    }

	setPredecessor(EmptyId) ;
	clearSuccList() ;
}

//-----------------------------------------------------------------------

Finger *FingerTable::getFinger(int position) 
{
	return &fingers[position-1];
}

//-----------------------------------------------------------------------

int FingerTable::getTableLength()
{
	return NBITS ;
}
//-----------------------------------------------------------------------

void FingerTable::setFinger(int position, Finger finger) 
{
	fingers[position-1] = finger;
}

//-----------------------------------------------------------------------

int FingerTable::getSuccNum() 
{
	for(int i = 0  ; i < NSUCCLIST ; i++)
	{
		if (successor[i].GetId() == -1)
			return i ;
	}
	return NSUCCLIST ;
}
//-----------------------------------------------------------------------

void FingerTable::clearSuccList() 
{
	ChordId EmptyId ;
	for(int i = 0 ; i < NSUCCLIST ; i++)
	{
		setSuccessor(i,EmptyId) ;
	}
}

//-------------------------------------------------------------------
ChordId FingerTable::getPredecessor() 
{
   return predecessor ;
}

//-------------------------------------------------------------------

void FingerTable::setPredecessor(ChordId pred) 
{
	this->predecessor = pred ;
}

//-------------------------------------------------------------------

ChordId FingerTable::getSuccessor(int pos)
{
	return successor[pos] ;
}

//-------------------------------------------------------------------

void FingerTable::setSuccessor(int pos , ChordId succ) 
{
	successor[pos] = succ ;
}

//-----------------------------------------------------------------------

int FingerTable::toString(char **buf , unsigned int &length) 
{
	/**/
	char *buffer = new char[4000] ;
	if (buffer==NULL)
	  return -1;

	char *message = buffer ;

	sprintf(message,"**********FINGER TABLE**********") ;
	message = message + strlen (message);
	strncpy (message, "\r\n\0", 2);
	message = message + 2;

	for(int i = 0 ; i < getTableLength() ; i++)
	{
		sprintf (message, "finger[%d]:	node:%d		start:%d	interval:%d~%d", 
			i+1,
			getFinger(i+1)->node.GetId() ,
			getFinger(i+1)->start.GetId() ,
			getFinger(i+1)->interval[0].GetId() ,
			getFinger(i+1)->interval[1].GetId() 
			);
		message = message + strlen (message);
		strncpy (message, "\r\n\0", 2);
		message = message + 2;
	}

	sprintf(message,"**********SUCC LIST**********") ;
	message = message + strlen (message);
	strncpy (message, "\r\n\0", 2);
	message = message + 2;

	for(i = 0 ; i < getSuccNum() ; i++)
	{
		sprintf (message, "SUCCLIST[%d]:	id:%d		ip:%s", 
			i,
			getSuccessor(i).GetId() ,
			getSuccessor(i).GetAddress() 
			);
		message = message + strlen (message);
		strncpy (message, "\r\n\0", 2);
		message = message + 2;
	}

	sprintf(message,"\r\n**********PREDECESSOR**********") ;
	message = message + strlen (message);
	strncpy (message, "\r\n\0", 2);
	message = message + 2;

	sprintf (message, "PREDECESSOR:		id:%d		ip:%s", 
			getPredecessor().GetId() ,
			getPredecessor().GetAddress() 
			);
	message = message + strlen (message);
	strncpy (message, "\r\n\0", 2);
	message = message + 2;

	strncpy(message,"\0",1) ;
	*buf = buffer ;
	length = strlen(buffer) ;
	return 0 ;
}

//***********************************************************************


