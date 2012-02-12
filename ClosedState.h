/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __CLOSEDSTATE_H__
#define __CLOSEDSTATE_H__

#include "NodeState.h"
/**
 * Initial state of the state machine.
 *
 * Design Pattern State.
 *
 */

class ClosedState : public NodeState 
{
public:
	virtual void Init(Node*) ;

	/*dispatch message*/
	virtual void Handle(Node*,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) ;
public :
	ClosedState() ;
	~ClosedState() ;
};

#endif	//__NODESTATE_H__

