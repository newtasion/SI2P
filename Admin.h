/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __ADMIN_H__
#define __ADMIN_H__

#include "ThreadTransport.h"
#include "ThreadDealMsg.h"
#include "AdOsip.h"

class Admin
{
public :
	ThreadTransport *th_transport ;
	ThreadDealMsg *th_dealmsg ;
	AdOsip *adosip;		
public:
	char localip[50];	/* default local ip */
	int localport;
	int ip_family ;
	struct osip_mutex *gl_lock;

public :
	int Admin_event_add_sip_message (osip_event_t * evt) ;
public :
	Admin() ;
	~Admin() ;
	int Admin_start(void) ;	//Æô¶¯threads

	/* the thread funcs are as follows */
	friend void *mythread_transport(void *arg) ;
	friend void *mythread_dealmsg(void *arg) ;
	friend void *mythread_adosip(void *arg) ;
} ;

extern Admin *admin ;



#endif //void head recontain