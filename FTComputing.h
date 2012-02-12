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

class FtComputingState : public NodeState 
{
public:

	virtual void on(NodeListener::FindSuccOK,NodeFsm *node) ;
};

#endif	//__NODESTATE_H__