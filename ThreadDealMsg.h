/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __THREADDEALMSG_H__
#define __THREADDEALMSG_H__

#include <osip2/osip.h>
#include <osipparser2/osip_parser.h>
#include <osip2/osip_mt.h>
#include "ThreadBase.h"

class ThreadDealMsg :public ThreadBase
{
protected:
	void *admin_core ;
	osip_fifo_t *sip_traffic;          /* New transaction_t element. */

public :
	ThreadDealMsg() ;	/*constructor*/
	int dealmsg_add_traffic(osip_transaction_t * inc_tr) ;

	int build_response() ;
public:
	int DealMsg_execute(int sec_max, int usec_max, int max_analysed) ;
	int step1() ;
} ;


#endif //void head recontain