/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820


#ifndef __REGAPI_H__
#define __REGAPI_H__

#include <osip2/osip.h>
#include <osip2/osip_dialog.h>
#include <osip2/osip_negotiation.h>
#include "Node.h"

enum REG_TYPE ;
class Node ;
class reg_t 
{
public:
	static int inc_rid;				/**/

	int						r_id ;
	int						r_reg_period;				/* delay between registration */
	REG_TYPE				reg_type ;				  /*type of register*/
	osip_transaction_t*		r_last_tr;
	
	/*record*/
	ChordId					req_uri ;
	ChordId					to ;
	ChordId					contact	;		 
	BOOL					IncludeSuccList ;

	Node*					node ;
	reg_t*					next;
	reg_t*					parent;
public:
	reg_t(Node *n);
	~reg_t();
};

//



#endif
