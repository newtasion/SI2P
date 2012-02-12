/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __FINGERTABLE_H__
#define __FINGERTABLE_H__

#include "baseunit.h"
#include "ChordId.h"
#include "Constants.h"

/*the struct of FingerTable Entry*/
struct Finger
{
	ChordId node;
    ChordId start;
    ChordId interval[2];
} ;

class Node ;
class FingerTable
{

private:
	
	Finger fingers[NBITS] ;

	void *constants ;

	ChordId				predecessor;
	ChordId				successor[NSUCCLIST];

	Node *localnode ;			/*the local node*/


public:
	FingerTable() {};
	FingerTable(void *cons, Node * node) ;
	~FingerTable() ;
	void initfingers() ;
	Finger *getFinger(int position) ;
	int getTableLength() ;
	void setFinger(int position, Finger finger)  ;

public :
	ChordId getPredecessor() ;
	void setPredecessor(ChordId pred)  ;
	ChordId getSuccessor(int pos) ;	/*0~NSUCCLIST*/
	void setSuccessor(int pos , ChordId succ) ;
	int getSuccNum() ;
	void clearSuccList() ;


public :
	

	int toString(char **buf , unsigned int &length) ;
} ;


#endif