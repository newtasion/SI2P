/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include "NodeState.h"

//-------------------------------------------------------------

void NodeState::ChangeState(Node* n, NodeState* s) 
{
	n->ChangeState(s) ;
}

//-------------------------------------------------------------

NodeState::kind NodeState::getKind() const 
{
	return _kind;
}

//-------------------------------------------------------------
int NodeState::toString(char **buf , unsigned int &length) 
{
	char *buffer = new char[100] ;
	if (buffer==NULL)
	  return -1;

	char *message = buffer ;

	sprintf(message,"**********NODEFSM STATE**********") ;
	message = message + strlen (message);
	strncpy (message, "\r\n\0", 2);
	message = message + 2;

	char * KindName[7] = 
	{
		{"CLOSESTATE"},
		{"INITEDSTATE"},
		{"DISCOVINGSTATE"},
		{"DISCOVEREDSTATE"},
		{"JOININGSTATE"},
		{"FTCOMPUTINGSTATE"},
		{"NORMALSTATE"}
	} ;

	sprintf(message,"NODEFSM STATE : %s",KindName[_kind]) ;

	message = message + strlen (message);
	strncpy (message, "\r\n\0", 2);
	message = message + 2;

	strncpy(message,"\0",1) ;
	*buf = buffer ;
	length = strlen(buffer) ;
	return 0 ;
}