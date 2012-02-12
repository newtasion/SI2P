/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include "InitedState.h"
#include "DiscovingState.h"

//----------------------------------------------------------------------

InitedState::InitedState()
{
	_kind = NodeState::INITEDSTATE ;
	printf("SWITCH TO INITEDSTATE !\n") ;
}

//----------------------------------------------------------------------

InitedState::~InitedState()
{
	printf("OUT OF INITEDSTATE !\n") ;
}

//----------------------------------------------------------------------
void InitedState::Discover(Node *node)
{
	//-	1.Snd MutilCast

	ChordId chordId = node->getChordId() ;
	node->SndRegister(MULTICAST,chordId,chordId,chordId) ; 

	//- 2.change state

	ChangeState(node,new DiscovingState()) ;

}

//----------------------------------------------------------------------

void InitedState::Handle(Node* node ,
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
		Discover(node) ;
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