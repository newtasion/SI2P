/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __THREADTRANSPORT_H__
#define __THREADTRANSPORT_H__


#ifndef OSIP_MT
#define OSIP_MT
#endif

#include <osip2/osip.h>
#include <osipparser2/osip_parser.h>
#include <osip2/osip_mt.h>

#include "baseunit.h"
#include "ThreadBase.h"
#include "Node.h"

class Node ;
class ThreadTransport :public ThreadBase
{
protected:
	//int sec_max ;
	//int usec_max ;
	int in_port;
    int in_socket;
	int out_port;
    int out_socket;
    int mcast_socket;

	Node *admin_core ;
public:
    /*侦听并接收消息*/
	int tran_rcv_message (int max) ;

	int	tran_snd_message (osip_transaction_t * transaction,	/* read only element */
		    osip_message_t * message,	/* message to send           */
		    char *host,	/* proposed destination host */
		    int port,	/* proposed destination port */
		    int socket)	;/* proposed socket (if any)  */

	/* 处理消息 */
	int tran_process_message (char *buf, char *ip_addr, int port, int length) ;

private :
	int __osip_message_fix_last_via_header (osip_message_t * request, char *ip_addr, int port) ;
public :
	/*constructor*/
	ThreadTransport(Node *node) ;
	~ThreadTransport() ;

	int Transport_execute(int sec_max, int usec_max, int max_analysed) ;
} ;

#endif //void head recontain