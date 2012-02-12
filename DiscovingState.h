/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __DISCOVINGSTATE_H__
#define __DISCOVINGSTATE_H__

#include "NodeState.h"
/**
 * Initial state of the state machine.
 *
 * Design Pattern State.
 *
 */

class DiscovingState : public NodeState 
{
public:

	virtual void onDiscoverOK(
					Node*,
					osip_transaction_t * tr ,
					osip_message *response) ;
	
	virtual void onNoNodeFind(
					Node *,
					osip_transaction_t *tr
					);

	/*dispatch message*/
	virtual void Handle(Node*,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) ;

public :
	DiscovingState() ;
	~DiscovingState() ;	
};

#endif	//__INITEDSTATE_H__