/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include "ClosedState.h"
#include "InitedState.h"

//----------------------------------------------------------------------

ClosedState::ClosedState()
{
	_kind = NodeState::CLOSESTATE ;
	printf("SWITCH TO CLOSEDSTATE !\n") ;
}

//----------------------------------------------------------------------

ClosedState::~ClosedState()
{
	printf("OUT OF CLOSEDSTATE!\n") ;
}

//----------------------------------------------------------------------

void ClosedState::Init(Node *node)
{
	//-	1.- init fingertable -//
	node->getFingerTable()->initfingers() ;

	//- 2.start adosip , thransport thread -//
	node->startthread() ;

	//- 3.state change to Inited -//
	ChangeState(node,new InitedState()) ;

	//- 4.log -//
}

//----------------------------------------------------------------------
void ClosedState::Handle(Node* node ,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) 
{
	//
	switch(event)
	{
	case INIT_SND :
		Init(node) ;
		break ;

	case DISCOVER_SND :
		break ;

	case JOIN_SND :
		break ;

	case LEAVE_SND :
		break ;

	case STABILIZE_SND :
		break ;


	case MULTICAST_REQ_RCV:
		break ;
	
	case MULTICAST_RESP_2XX:
		break ;
	
	case JOIN_REQ_RCV:
		break ;

	case JOIN_RESP_2XX:
		break ;

	case JOIN_RESP_3XX:
		break ;

	case FINDSUCC_REQ_RCV:
		break ;

	case FINDSUCC_RESP_2XX:
		break ;

	case FINDSUCC_RESP_3XX:
		break ;

	case STABILIZE_REQ_RCV:
		break ;

	case STABILIZE_RESP_2XX:
		break ;

	case LEAVE_REQ_RCV:
		break ;
	
	case EVT_OTHER:
		break ;
	
	}
}