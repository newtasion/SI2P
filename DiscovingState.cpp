/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include "DiscovingState.h"
#include "DiscoveredState.h"
#include "BootStraps.h"
#include "pubtool.h"
#include "NormalState.h"

//----------------------------------------------------------------------

DiscovingState::DiscovingState()
{
	_kind = NodeState::DISCOVINGSTATE ;
	printf("SWITCH TO DISCOVINGSTATE !\n") ;
}

//----------------------------------------------------------------------

DiscovingState::~DiscovingState()
{
	printf("OUT OF DISCOVINGSTATE !\n") ;
}

//------------------------------------------------------------------

void DiscovingState::onDiscoverOK(
					Node* node,
					osip_transaction_t * tr ,
					osip_message *response) 
{
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(response, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;	
	//- 1. store bootstraps

	osip_contact_t *osip_contact ;
	osip_message_get_contact(response,0,&osip_contact) ;
	if(!osip_contact)	
	{return ;}//error

	ChordId contact(
		atoi(osip_contact->url->username),
		node->getConstants(),
		osip_contact->url->host,
		osip_contact->url->port
		) ;
	
	BootStrap *bs = new BootStrap(
		atoi(osip_contact->url->username) , 
		osip_contact->url->host,
		osip_contact->url->port
		);

	ADD_ELEMENT(node->j_bootstraps, bs) ;


	//- 2. change state

	NodeState* ns=new DiscoveredState();
	ChangeState(node,ns) ;

	//- 3. join the overlay

	ns->Handle(node,JOIN_SND,NULL,NULL);
}
//----------------------------------------------------------------------

void DiscovingState::onNoNodeFind(Node * node,osip_transaction_t *tr)
{
	//1.use other bootstrap node


	//2.boot as the first node in the overlay
	
	ChordId chordId=node->getChordId();
	node->getFingerTable()->setPredecessor(chordId) ;
	node->getFingerTable()->setSuccessor(0,chordId) ;
	node->Set_Fingers(1,chordId)  ;
	 
	//-直接进入normal状态
	ChangeState(node,new NormalState(node)) ;



}
//----------------------------------------------------------------------
void DiscovingState::Handle(Node* node ,
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
		//- DiscovingState消息响应，驱动进入DiscoveredState状态
		onDiscoverOK(
			node,
			tr ,
			message) ;

		break ;
	case NO_NODE_FIND:
		
		onNoNodeFind(node,tr);
		break;
	
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
