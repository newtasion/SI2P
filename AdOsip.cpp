/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820
#ifndef OSIP_MT
#define OSIP_MT
#endif

#include "AdOsip.h"
#include "callback.h"
#include "event.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <osip2/osip_mt.h>
#include <osip2/osip_condv.h>
#include <osip2/osip.h>
#ifdef __cplusplus
}
#endif


AdOsip::AdOsip()
{
	int i;

	/* initialize osip */
	i = osip_init (&j_osip);
	if (i != 0)
		return;
	
	/**/
	j_cond      = (struct osip_cond*)osip_cond_init();
	j_mutexlock = (struct osip_mutex *)osip_mutex_init();

	j_transactions = (osip_list_t*) osip_malloc(sizeof(osip_list_t));
	osip_list_init(j_transactions);

	/* set the callback */
	AdOsip_Set_Callback();

	delay = 500;
	exit_flag = 0;	/* 0 not set, 1 set */
	//timers = NULL;
}

//----------------------------------------------------------------------
AdOsip::~AdOsip() 
{

	exit_flag = 1;
	osip_mutex_destroy((struct osip_mutex*)j_mutexlock);
	osip_cond_destroy((struct osip_cond*)j_cond);

	while (!osip_list_eol(j_transactions, 0))
    {
		osip_transaction_t *tr = (osip_transaction_t*) osip_list_get(j_transactions, 0);
		if (tr->state==IST_TERMINATED || tr->state==ICT_TERMINATED
		|| tr->state== NICT_TERMINATED || tr->state==NIST_TERMINATED)
		{
			OSIP_TRACE(osip_trace(__FILE__,__LINE__,OSIP_INFO1,NULL,"Release a terminated transaction\n"));
			osip_list_remove(j_transactions, 0);
			osip_transaction_free(tr);
	}
    else
	{
		osip_list_remove(j_transactions, 0);
		osip_transaction_free(tr);
	}
    }

	osip_free(j_transactions);

	_osip_kill_transaction (j_osip->osip_ict_transactions);
	_osip_kill_transaction (j_osip->osip_nict_transactions);
	_osip_kill_transaction (j_osip->osip_ist_transactions);
	_osip_kill_transaction (j_osip->osip_nist_transactions);
	osip_release (j_osip);

	return ;
}
//----------------------------------------------------------------------
osip_t *AdOsip::GetOsip() 
{
	return j_osip ;
}
//----------------------------------------------------------------------
int AdOsip::AdOsip_lock() 
{
	return osip_mutex_lock((struct osip_mutex*)j_mutexlock);
}
//------------------------------------------------------------------------
int AdOsip::AdOsip_unlock() 
{
	return osip_mutex_unlock((struct osip_mutex*)j_mutexlock);
}

//------------------------------------------------------------------------
int AdOsip::AdOsip_Set_Callback() 
{
	osip_set_cb_send_message (j_osip, ad_cb_snd_message);

	osip_set_kill_transaction_callback (j_osip, OSIP_ICT_KILL_TRANSACTION, ad_cb_ict_kill_transaction);
	osip_set_kill_transaction_callback (j_osip, OSIP_IST_KILL_TRANSACTION, ad_cb_ist_kill_transaction);
	osip_set_kill_transaction_callback (j_osip, OSIP_NICT_KILL_TRANSACTION, ad_cb_nict_kill_transaction);
	osip_set_kill_transaction_callback (j_osip, OSIP_NIST_KILL_TRANSACTION, ad_cb_nist_kill_transaction);

	osip_set_transport_error_callback (j_osip, OSIP_ICT_TRANSPORT_ERROR, ad_cb_transport_error);
	osip_set_transport_error_callback (j_osip, OSIP_IST_TRANSPORT_ERROR, ad_cb_transport_error);
	osip_set_transport_error_callback (j_osip, OSIP_NICT_TRANSPORT_ERROR, ad_cb_transport_error);
	osip_set_transport_error_callback (j_osip, OSIP_NIST_TRANSPORT_ERROR, ad_cb_transport_error);

	osip_set_message_callback (j_osip, OSIP_ICT_STATUS_1XX_RECEIVED, ad_cb_rcv1xx);
	osip_set_message_callback (j_osip, OSIP_ICT_STATUS_2XX_RECEIVED, ad_cb_rcv2xx);
	osip_set_message_callback (j_osip, OSIP_ICT_STATUS_3XX_RECEIVED, ad_cb_rcv3xx);
	osip_set_message_callback (j_osip, OSIP_ICT_STATUS_4XX_RECEIVED, ad_cb_rcv4xx);
	osip_set_message_callback (j_osip, OSIP_ICT_STATUS_5XX_RECEIVED, ad_cb_rcv5xx);
	osip_set_message_callback (j_osip, OSIP_ICT_STATUS_6XX_RECEIVED, ad_cb_rcv6xx);

	osip_set_message_callback (j_osip, OSIP_IST_STATUS_1XX_SENT, ad_cb_snd1xx);
	osip_set_message_callback (j_osip, OSIP_IST_STATUS_2XX_SENT, ad_cb_snd2xx);
	osip_set_message_callback (j_osip, OSIP_IST_STATUS_3XX_SENT, ad_cb_snd3xx);
	osip_set_message_callback (j_osip, OSIP_IST_STATUS_4XX_SENT, ad_cb_snd4xx);
	osip_set_message_callback (j_osip, OSIP_IST_STATUS_5XX_SENT, ad_cb_snd5xx);
	osip_set_message_callback (j_osip, OSIP_IST_STATUS_6XX_SENT, ad_cb_snd6xx);

	osip_set_message_callback (j_osip, OSIP_NICT_STATUS_1XX_RECEIVED, ad_cb_rcv1xx);
	osip_set_message_callback (j_osip, OSIP_NICT_STATUS_2XX_RECEIVED, ad_cb_rcv2xx);
	osip_set_message_callback (j_osip, OSIP_NICT_STATUS_3XX_RECEIVED, ad_cb_rcv3xx);
	osip_set_message_callback (j_osip, OSIP_NICT_STATUS_4XX_RECEIVED, ad_cb_rcv4xx);
	osip_set_message_callback (j_osip, OSIP_NICT_STATUS_5XX_RECEIVED, ad_cb_rcv5xx);
	osip_set_message_callback (j_osip, OSIP_NICT_STATUS_6XX_RECEIVED, ad_cb_rcv6xx);

	osip_set_message_callback (j_osip, OSIP_NIST_STATUS_1XX_SENT, ad_cb_snd1xx);
	osip_set_message_callback (j_osip, OSIP_NIST_STATUS_2XX_SENT, ad_cb_snd2xx);
	osip_set_message_callback (j_osip, OSIP_NIST_STATUS_3XX_SENT, ad_cb_snd3xx);
	osip_set_message_callback (j_osip, OSIP_NIST_STATUS_4XX_SENT, ad_cb_snd4xx);
	osip_set_message_callback (j_osip, OSIP_NIST_STATUS_5XX_SENT, ad_cb_snd5xx);
	osip_set_message_callback (j_osip, OSIP_NIST_STATUS_6XX_SENT, ad_cb_snd6xx);

	osip_set_message_callback (j_osip, OSIP_IST_INVITE_RECEIVED, ad_cb_rcvinvite);
	osip_set_message_callback (j_osip, OSIP_IST_ACK_RECEIVED, ad_cb_rcvack);
	osip_set_message_callback (j_osip, OSIP_IST_ACK_RECEIVED_AGAIN, ad_cb_rcvack2);
	osip_set_message_callback (j_osip, OSIP_NIST_REGISTER_RECEIVED, ad_cb_rcvregister);
	osip_set_message_callback (j_osip, OSIP_NIST_BYE_RECEIVED, ad_cb_rcvbye);
	osip_set_message_callback (j_osip, OSIP_NIST_CANCEL_RECEIVED, ad_cb_rcvcancel);
	osip_set_message_callback (j_osip, OSIP_NIST_INFO_RECEIVED, ad_cb_rcvinfo);
	osip_set_message_callback (j_osip, OSIP_NIST_OPTIONS_RECEIVED, ad_cb_rcvoptions);
	osip_set_message_callback (j_osip, OSIP_NIST_SUBSCRIBE_RECEIVED, ad_cb_rcvoptions);
	osip_set_message_callback (j_osip, OSIP_NIST_NOTIFY_RECEIVED, ad_cb_rcvoptions);
	osip_set_message_callback (j_osip, OSIP_NIST_UNKNOWN_REQUEST_RECEIVED, ad_cb_rcvunkrequest);
	return 0 ;
}

//---------------------------------------------------------------------------------
void AdOsip::AdOsip_set_delay(int dly) 
{
	delay = dly ;
}

//-----------------------------------------------------------------------------------
void AdOsip::_osip_kill_transaction (osip_list_t * transactions) 
{
	osip_transaction_t *transaction;

	if (!osip_list_eol (transactions, 0))
    {
		/* some transaction are still used by osip,
         transaction should be released by modules! */
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "_osip_kill_transaction transaction should be released by modules!\n"));
    }

	while (!osip_list_eol (transactions, 0))
    {
		transaction = (osip_transaction *)osip_list_get (transactions, 0);
		osip_transaction_free (transaction);
    }
}

//-----------------------------------------------------------------------------------
int AdOsip::AdOsip_execute() 
{
	fd_set osip_fdset;
	struct timeval tv;

	while (1)
    {
		int s;
		int i;

		tv.tv_sec = 0;
		tv.tv_usec = delay * 1000;
	
		FD_ZERO (&osip_fdset);
		s = wakeup.jpipe_get_read_descr() ;
		FD_SET (s, &osip_fdset);

		/*×èÈû¼àÌý*/
		i = select (s + 1, &osip_fdset, NULL, NULL, &tv);

		if (FD_ISSET (s, &osip_fdset))
        {
			char tmp[51];

			i = wakeup.jpipe_read(tmp,50) ;
			if (i == 1 && tmp[0] == 'q')
            {
				OSIP_TRACE (osip_trace
                          (__FILE__, __LINE__, OSIP_INFO2, NULL,
                           "osip module: Exiting!\n"));
				return 0;
			} 
			else
            {
				OSIP_TRACE (osip_trace
                          (__FILE__, __LINE__, OSIP_INFO2, NULL,
                           "osip module: wake up!\n"));
            }
        }

      /*      susleep (); */
      
	  osip_timers_ict_execute (j_osip);
      osip_timers_ist_execute (j_osip);
      osip_timers_nict_execute (j_osip);
      osip_timers_nist_execute (j_osip);

      AdOsip_lock ();
      osip_ict_execute (j_osip);
      osip_nict_execute (j_osip);
      osip_ist_execute (j_osip);
      osip_nist_execute (j_osip);
      AdOsip_unlock ();

      if (exit_flag == 1)
			return NULL;
    }
	return 0;
}
//---------------------------------------------------------------------