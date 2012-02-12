/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __NORMALSTATE_H__
#define __NORMALSTATE_H__

#include "NodeState.h"
/**
 * Initial state of the state machine.
 *
 * Design Pattern State.
 *
 */

class NormalState : public NodeState 
{
public:
	NormalState(Node *) ;
	virtual ~NormalState() ;

	/*dispatch message*/
	virtual void Handle(Node*,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) ;

	virtual void Leave(Node*) ;

	virtual void Stabilize(Node *) ;

	//--------------------------------------------------

	virtual void onFindSuccOK(
					Node*,
					osip_transaction_t * tr ,
					osip_message *response) ;

	virtual void onStabilizeOK(
					Node*,
					osip_transaction_t * tr ,
					osip_message *response) ;

	//--------------------------------------------------
	
	virtual void onFindSuccRedirect(
					Node*,
					osip_transaction_t * tr ,
					osip_message *response) ;


	//--------------------------------------------------

	virtual void onDiscoverReg( 
					Node*,
					osip_transaction_t *tr,
					osip_message_t *request ) ;

	virtual void onJoinReg( 
					Node*,
					osip_transaction_t *tr,
					osip_message_t *request ) ;

	virtual void onFindSuccReg( 
					Node*,
					osip_transaction_t *tr,
					osip_message_t *request ) ;

	virtual void onStabilizeReg( 
					Node*,
					osip_transaction_t *tr,
					osip_message_t *request ) ;

	virtual void onLeaveReg(
					Node*,
					osip_transaction_t *tr,
					osip_message_t *request ) ;
	//--------------------------------------------------
	virtual void onUserReg(Node *node,
							osip_transaction_t *tr,
							osip_message_t *request);

	virtual void onUserRegOK(
					Node *,
					osip_transaction_t *tr,
					osip_message_t *response);
	
	virtual void onUserRegRedirect(
					Node *,
					osip_transaction_t *tr,
					osip_message_t *response);

	//--------------------------------------------------
	virtual void onUserQuery(Node *node,
							osip_transaction_t *tr,
							osip_message_t *request);

	virtual void onUserQueryOK(
					Node *,
					osip_transaction_t *tr,
					osip_message_t *response);
	
	virtual void onUserQueryRedirect(
					Node *,
					osip_transaction_t *tr,
					osip_message_t *response);

	virtual void onNodeFailure(
					Node *,
					osip_transaction_t *tr);

	virtual void onRedRegister(
					Node *node,
					osip_transaction_t *tr,
					osip_message_t *message);

	virtual void onRedRegisterOK(
					Node *node,
					osip_transaction_t *tr,
					osip_message_t *message);
	//--------------------------------------------------

private :

	
};

#endif	//__INITEDSTATE_H__