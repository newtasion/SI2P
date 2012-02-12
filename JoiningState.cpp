/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include "JoiningState.h"
#include "NormalState.h"
#include "FTComputingState.h"

//----------------------------------------------------------------------

JoiningState::JoiningState()
{
	_kind = NodeState::JOININGSTATE ;
	printf("SWITCH TO JOININGSTATE !\n") ;
}

//----------------------------------------------------------------------

JoiningState::~JoiningState()
{
	printf("OUT OF JOININGSTATE !\n") ;
}

//------------------------------------------------------------------
void JoiningState::onJoinOK(
					Node* node,
					osip_transaction_t * tr ,
					osip_message *response)
{
	
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(response, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
	printf("RECEIVE<<======================================\n") ;
	printf(message) ;

	ChordId to(
			atoi(response->to->url->username),
			node->getConstants(),
			response->to->url->host,
			response->to->url->port
			) ;

	osip_contact_t * osip_contact ;
	osip_message_get_contact(response,0,&osip_contact) ;
	if(!osip_contact)	
	{return ;}//error
	ChordId contact(
		atoi(osip_contact->url->username),
		node->getConstants(),
		osip_contact->url->host,
		osip_contact->url->port
		) ;
	
	ChordId chordId = node->getChordId() ;
	//***********************************

	int k = node->Set_Fingers(1,contact) + 1 ;	
	node->setSuccListWithContacts(response,true) ;
	node->setPredWithContacts(response) ;
	
	/* state convert to  FTComputingState */
	if (k <= node->getFingerTable()->getTableLength())		//update of fingertable is not completed
	{
		int i = node->SndRegister(FINDSUCC,contact,node->getFingerTable()->getFinger(k)->start,chordId) ; 
		
		//- change state ...
		ChangeState(node,new FTComputingState()) ;
		return ;
	}
	
	/* state convert to  NormalState */
	if (k == node->getFingerTable()->getTableLength() + 1)
	{
		
		//- Notify ...
		Notify(node) ;		
		//- change state ...
		ChangeState(node,new NormalState(node)) ;
		
		return ;

	}

}

//------------------------------------------------------------------

void JoiningState::Notify(Node* node)
{
	ChordId chordId = node->getChordId() ;

	//mynode->stabilizing = 1 ;
	int i = node->SndRegister(
		STABILIZE,
		node->getFingerTable()->getSuccessor(0),
		node->getFingerTable()->getSuccessor(0),
		chordId) ; 

	if(!node->getFingerTable()->getPredecessor().equals(node->getFingerTable()->getSuccessor(0)) 
		&& !node->getFingerTable()->getPredecessor().IsEmptyChordId())
	{
		//mynode->stabilizing = 1 ;
		int i = node->SndRegister(STABILIZE,
			node->getFingerTable()->getPredecessor(),
			node->getFingerTable()->getPredecessor(),
			chordId) ; 
	}
}

//------------------------------------------------------------------

void JoiningState::onJoinRedirect(
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

	/* to , from , contact are same as orig_request ;
	req_uri is the contact of the response */
	ChordId to(
			atoi(tr->orig_request->to->url->username),
			node->getConstants(),
			tr->orig_request->to->url->host,
			tr->orig_request->to->url->port
			) ;
	ChordId from(
			atoi(tr->orig_request->from->url->username),
			node->getConstants(),
			tr->orig_request->from->url->host,
			tr->orig_request->from->url->port
			) ;

		
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


	ChordId chordId = node->getChordId() ;
	//******************************


	node->SndRegister(JOIN,contact,to,chordId) ; 	
}
//------------------------------------------------------------------

void JoiningState::Handle(Node* node ,
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
		//- JoiningState消息响应，驱动进入FTComputingState/NormalState状态
		onJoinOK(
			node,
			tr ,
			message) ;

		break ;

	case JOIN_RESP_3XX:
		//- JoiningState消息响应，继续JoiningState状态
		onJoinRedirect(
			node,
			tr ,
			message) ;

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