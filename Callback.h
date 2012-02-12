/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include <osip2/osip.h>

//------------------------------------------------------------------------ 
/* callbacks */
int ad_cb_snd_message (osip_transaction_t *tr, osip_message_t *sip, char *host,
		       int port, int out_socket) ;
void ad_cb_ict_kill_transaction (int type, osip_transaction_t * tr) ;
void ad_cb_nist_kill_transaction (int type, osip_transaction_t * tr) ;
void ad_cb_ist_kill_transaction (int type, osip_transaction_t * tr) ;
void ad_cb_nict_kill_transaction (int type, osip_transaction_t * tr) ;
void ad_cb_transport_error (int type, osip_transaction_t *tr, int error) ;
void ad_cb_rcv1xx (int type, osip_transaction_t * tr, osip_message_t * sip) ;
void ad_cb_rcv2xx (int type, osip_transaction_t * tr, osip_message_t * sip) ;
void ad_cb_rcv3xx (int type, osip_transaction_t * tr, osip_message_t * sip) ;
void ad_cb_rcv4xx (int type, osip_transaction_t * tr, osip_message_t * sip) ;
void ad_cb_rcv5xx (int type, osip_transaction_t * tr, osip_message_t * sip) ;
void ad_cb_rcv6xx (int type, osip_transaction_t * tr, osip_message_t * sip) ;
void ad_cb_rcvinvite (int type, osip_transaction_t * tr, osip_message_t * sip) ; 
void ad_cb_rcvack (int type, osip_transaction_t * tr, osip_message_t * sip);
void ad_cb_rcvack2 (int type, osip_transaction_t * tr, osip_message_t * sip);
void ad_cb_rcvregister (int type, osip_transaction_t * tr, osip_message_t * sip);
void ad_cb_rcvbye (int type, osip_transaction_t * tr, osip_message_t * sip);
void ad_cb_rcvcancel (int type, osip_transaction_t * tr, osip_message_t * sip);
void ad_cb_rcvinfo (int type, osip_transaction_t * tr, osip_message_t * sip);
void ad_cb_rcvoptions (int type, osip_transaction_t * tr, osip_message_t * sip);
void ad_cb_rcvprack (int type, osip_transaction_t * tr, osip_message_t * sip);
void ad_cb_rcvunkrequest (int type, osip_transaction_t * tr, osip_message_t * sip);
void ad_cb_snd1xx (int type, osip_transaction_t *tr,osip_message_t *sip);
void ad_cb_snd2xx (int type, osip_transaction_t *tr,osip_message_t *sip);
void ad_cb_snd3xx (int type, osip_transaction_t *tr,osip_message_t *sip);
void ad_cb_snd4xx (int type, osip_transaction_t *tr,osip_message_t *sip);
void ad_cb_snd5xx (int type, osip_transaction_t *tr,osip_message_t *sip);
void ad_cb_snd6xx (int type, osip_transaction_t *tr,osip_message_t *sip);



	

#endif