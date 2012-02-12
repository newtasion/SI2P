/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820
#include "DiscoveredState.h"
#include "JoiningState.h"

//----------------------------------------------------------------------

DiscoveredState::DiscoveredState()
{
	_kind = NodeState::DISCOVEREDSTATE ;
	printf("SWITCH TO DISCOVEREDSTATE !\n") ;
}

//----------------------------------------------------------------------

DiscoveredState::~DiscoveredState()
{
	printf("OUT OF DISCOVEREDSTATE !\n") ;
}

//---------------------------------------------------------------------------

void DiscoveredState::Join(Node* node)
{
	//- 1. Send Join message
	ChordId chordId = node->getChordId() ;

	/*get a bootstrap*/
	ChordId requri = node->getABootStrap() ;

	/* send join register message */
	node->SndRegister(JOIN,requri,requri,chordId) ; 	

	//- 2. Change State 
	
	ChangeState(node,new JoiningState()) ;
}

//----------------------------------------------------------------------
void DiscoveredState::Handle(Node* node ,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) 
{
	//
	switch(event)
	{
	case INIT_SND :
		break ;

	case DISCOVER_SND :
		break ;

	case JOIN_SND :
		Join(node) ;
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