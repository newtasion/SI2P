/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include "FTComputingState.h"
#include "NormalState.h"

//----------------------------------------------------------------------

FTComputingState::FTComputingState()
{
	_kind = NodeState::FTCOMPUTINGSTATE ;
	printf("SWITCH TO FTCOMPUTINGSTATE !\n") ;
}

//----------------------------------------------------------------------

FTComputingState::~FTComputingState()
{
	printf("OUT OF FTCOMPUTINGSTATE !\n") ;
}

//------------------------------------------------------------------

void FTComputingState::Notify(Node* node)
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
void FTComputingState::onFindSuccOK(	Node* node,
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
	//******************************

	//scan fingertable
	for(i = 1 ; i <= node->getFingerTable()->getTableLength() ; i++)
	{
		if(node->getFingerTable()->getFinger(i)->start.equals(to)) 
			break ;
	}
	
	int k ;
	if(i <= node->getFingerTable()->getTableLength())
		k = node->Set_Fingers(i, contact) + 1 ;

	//A.finger表更新完毕，有两种情况
	if (k == node->getFingerTable()->getTableLength() + 1)
	{
		//- Notify ...
		Notify(node) ;			
		//- change state ...
		ChangeState(node,new NormalState(node)) ;

		return ;
	}

	//B.finger表未更新完
	if (k<= node->getFingerTable()->getTableLength())	//update of fingertalbe if not completed
	{
		int i = node->SndRegister(FINDSUCC,contact,node->getFingerTable()->getFinger(k)->start,chordId) ;

		//- not change state...
		return ;
	}
}

//----------------------------------------------------------------------

void FTComputingState::onFindSuccRedirect(
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

	//如果重定向到自己,那么自身就是对应to的finger
	if(contact.equals(chordId))
	{
		//scan fingertable
		for(int i = 1 ; i <= node->getFingerTable()->getTableLength() ; i++)
		{
			if(node->getFingerTable()->getFinger(i)->start.equals(to)) 
				break ;
		}

		int k ;
		if(i <= node->getFingerTable()->getTableLength())
			k = node->Set_Fingers(i, contact) + 1 ;

		//A.finger表更新完毕
		if (k == node->getFingerTable()->getTableLength() + 1)
		{
			//- Notify ...
			Notify(node) ;			
			//- change state ...
			ChangeState(node,new NormalState(node)) ;
		}

		//B.finger表未更新完
		if (k<= node->getFingerTable()->getTableLength())	//update of fingertalbe if not completed
		{
			int i = node->SndRegister(FINDSUCC,contact,node->getFingerTable()->getFinger(k)->start,chordId) ; 
			
			//- not change state...
			return ;
		}	
	}

	//继续重定向
	else
	{
		node->SndRegister(FINDSUCC,contact,to,chordId) ; 		
	}

}

//----------------------------------------------------------------------

void FTComputingState::Handle(Node* node ,
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
		break ;

	case JOIN_RESP_3XX:
		break ;

	case FINDSUCC_REQ_RCV:
		break ;

	case FINDSUCC_RESP_2XX:

		onFindSuccOK( 
			node,
			tr ,
			message) ;
		break ;

	case FINDSUCC_RESP_3XX:

		onFindSuccRedirect( 
			node,
			tr ,
			message) ;
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