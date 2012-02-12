/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __JOININGSTATE_H__
#define __JOININGSTATE_H__

#include "NodeState.h"
/**
 * Initial state of the state machine.
 *
 * Design Pattern State.
 *
 */

class JoiningState : public NodeState 
{
private:
	//tell succ&&pred some infomation which can change datas of succ&&pred
	void Notify(Node* node) ;

public:

	virtual void onJoinOK(
					Node* node,
					osip_transaction_t * tr ,
					osip_message *response) ;

	virtual void onJoinRedirect(
					Node* node,
					osip_transaction_t * tr ,
					osip_message *response) ;

public:
	/*dispatch message*/
	virtual void Handle(Node*,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) ;

public :
	JoiningState() ;
	~JoiningState() ;
};

#endif	//__NODESTATE_H__