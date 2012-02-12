/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __FTCOMPUTINGSTATE_H__
#define __FTCOMPUTINGSTATE_H__

#include "NodeState.h"
/**
 * Initial state of the state machine.
 *
 * Design Pattern State.
 *
 */

class FTComputingState : public NodeState 
{
private :
	//tell succ&&pred some infomation which can change datas of succ&&pred
	void Notify(Node* node) ;

public:

	virtual void onFindSuccOK(	Node*,
					osip_transaction_t * tr ,
					osip_message *response) ;

	virtual void onFindSuccRedirect(
					Node* node,
					osip_transaction_t * tr ,
					osip_message *response) ;

	/*dispatch message*/
	virtual void Handle(Node*,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) ;

public :
	FTComputingState() ;
	~FTComputingState() ;
};

#endif	//__NODESTATE_H__