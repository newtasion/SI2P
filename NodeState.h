/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __NODESTATE_H__
#define __NODESTATE_H__

#include "MyType.h"
#include "NodeListener.h"
//#include "Speaker.h"
#include "Singleton.h"
#include "Node.h"

/**
 * Interface for the Node state machine.
 *
 *	Design Pattern State.
	Singleton Pattern State.
 */
class NodeState 
{
public:
	NodeState() {};
	virtual ~NodeState() {} ;
	/**
	 * @enum used to avoid casting
	 */
	enum kind {
		CLOSESTATE,
		INITEDSTATE,
		DISCOVINGSTATE,
		DISCOVEREDSTATE,
		JOININGSTATE,
		FTCOMPUTINGSTATE,
		NORMALSTATE
	};

public :
	NodeState::kind _kind;

public:
	/// return the kind of state
	NodeState::kind getKind() const;
	int toString(char **buf , unsigned int &length) ;

protected :
	/*
	[Close]--->[Inited]
	*/
	virtual void Init(Node*) {};

	/*
	[Inited]--->[Discoving]
	*/
	virtual void Discover(Node*) {};

	/*
	[Discovered]--->[Joining]
	*/
	virtual void Join(Node*) {};

	/*
	[Joined]--->[FTComputing]
	*/
	virtual void FTCompute(Node*) {};
	
	/*
	[Normal]--->[Close]
	*/
	virtual void Leave(Node*) {};

	/*
	[Normal]--->[Normal]
	*/
	virtual void Stabilize(Node*) {};

public :	

	/*dispatch message*/
	virtual void Handle(Node*,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) = 0;

protected :	
	
	virtual void onDiscoverOK(
					Node*,
					osip_transaction_t * tr ,
					osip_message *response)  { }

	virtual void onJoinOK(
					Node*,
					osip_transaction_t * tr ,
					osip_message *response)  { }

	virtual void onFindSuccOK(
					Node*,
					osip_transaction_t * tr ,
					osip_message *response)  { }

	virtual void onStabilizeOK(
					Node*,
					osip_transaction_t * tr ,
					osip_message *response)  { }

	//--------------------------------------------------

	virtual void onJoinRedirect(
					Node*,
					osip_transaction_t * tr ,
					osip_message *response)  { }
	
	virtual void onFindSuccRedirect(
					Node*,
					osip_transaction_t * tr ,
					osip_message *response)  { }


	//--------------------------------------------------

	virtual void onDiscoverReg( 
					Node*,
					osip_transaction_t *tr,
					osip_message_t *request ) {};

	virtual void onJoinReg( 
					Node*,
					osip_transaction_t *tr,
					osip_message_t *request ) {};

	virtual void onFindSuccReg( 
					Node*,
					osip_transaction_t *tr,
					osip_message_t *request ) {};

	virtual void onStabilizeReg( 
					Node*,
					osip_transaction_t *tr,
					osip_message_t *request ) {};

	virtual void onLeaveReg(
					Node*,
					osip_transaction_t *tr,
					osip_message_t *request ) {};

	//--------------------------------------------------
protected :

	void ChangeState(Node* , NodeState*) ;
};

#endif	//__NODESTATE_H__
