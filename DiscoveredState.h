/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __JOINSTATE_H__
#define __JOINSTATE_H__

#include "NodeState.h"
/**
 * Initial state of the state machine.
 *
 * Design Pattern State.
 *
 */

class DiscoveredState : public NodeState 
{
public:
	virtual void Join(Node*) ;

	/*dispatch message*/
	virtual void Handle(Node*,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) ;

public :
	DiscoveredState() ;
	~DiscoveredState() ;
};

#endif	//__JOINSTATE_H__