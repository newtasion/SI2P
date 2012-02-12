/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include "regapi.h"
#include "Node.h"

int reg_t::inc_rid = 0 ;
//------------------------------------------------------------------------------
reg_t::reg_t(Node *n)
{
	if (inc_rid > 1000000)			/* keep it non-negative */
  	inc_rid = 0;

	r_id			= ++inc_rid;
	r_reg_period	= 3600;      /* delay between registration */
	r_last_tr		= NULL;
	reg_type		= OTHER ;

	node			= n ;
	next			= NULL;
	parent			= NULL;
}

//------------------------------------------------------------------------------

reg_t::~reg_t()
{
	//remove the response tr to node.j_transactions
	if (r_last_tr != NULL)
    {
		if (r_last_tr->state==IST_TERMINATED ||
			r_last_tr->state==ICT_TERMINATED ||
			r_last_tr->state== NICT_TERMINATED ||
			r_last_tr->state==NIST_TERMINATED)
		{
			OSIP_TRACE(osip_trace(__FILE__,__LINE__,OSIP_INFO1,NULL,
			"Release a terminated transaction\n"));
			if (r_last_tr!=NULL)
				osip_list_add(node->j_transactions, r_last_tr, 0);
		}
		else
		{
			OSIP_TRACE(osip_trace(__FILE__,__LINE__,OSIP_INFO1,NULL,
			"Release a non-terminated transaction\n"));
			if (r_last_tr!=NULL)
				osip_list_add(node->j_transactions, r_last_tr, 0);
		}
    }
}

//------------------------------------------------------------------------------
