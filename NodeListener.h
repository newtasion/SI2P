/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __NODELISTENER_H__
#define __NODELISTENER_H__

class NodeListener {
public:
	template<int I>	struct X { enum { TYPE = I };  };

	typedef X<0> DiscoverOK ;
	typedef X<1> JoinOK ;
	typedef X<2> FindSuccOK ;
	typedef X<3> StabilizeOK ;
	typedef X<4> DiscoverReg ;
	typedef X<5> JoinReg ;
	typedef X<6> FindSuccReg ;
	typedef X<7> StabilizeReg ;

};

#endif //__NODELISTENER_H__