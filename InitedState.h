/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __INITEDSTATE_H__
#define __INITEDSTATE_H__

#include "NodeState.h"
/**
 * Initial state of the state machine.
 *
 * Design Pattern State.
 *
 */

class InitedState : public NodeState 
{
public:

	virtual void Discover(Node*) ;

	/*dispatch message*/
	virtual void Handle(Node*,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) ;

public :
	InitedState() ;
	~InitedState() ;
};

#endif	//__INITEDSTATE_H__