/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0822

#include "Node.h"
#include "AdOsip.h"

/*void AdOsip::report_event_with_status(event_t *je, osip_message_t *sip)
{
	if (je!=NULL)
    {
		int evt = je->etype;

		if (sip != NULL)
			je->event_add_status(sip);
		if (node.j_call_callbacks[evt]!=NULL)
			node.j_call_callbacks[evt](evt, je);
		node.event_add(je);
    }
}*/
//-----------------------------------------------------------------------------
int ad_cb_snd_message (osip_transaction_t * tr, osip_message_t * sip, char *host,
			 int port, int out_socket)
{
	
	/* 调用传输层发送响应 */
	return mynode->th_transport->tran_snd_message (tr, sip, host, port, out_socket);
}


//-----------------------------------------------------------------------------
void ad_cb_ict_kill_transaction (int type, osip_transaction_t * tr)
{
  static int ict_killed = 0;

  ict_killed++;
  if (ict_killed % 100 == 0)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_INFO1, NULL,
		   "Number of killed ICT transaction = %i\n", ict_killed));
    }
  osip_remove_transaction ((osip_t *) tr->config, tr);
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_ict_kill_transaction!\n"));
}
//-----------------------------------------------------------------------------
void ad_cb_nist_kill_transaction (int type, osip_transaction_t * tr)
{
	static int nist_killed = 0;

	nist_killed++;
	if (nist_killed % 100 == 0)
    {
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_INFO1, NULL,
		   "Number of killed NIST transaction = %i\n", nist_killed));
    }
	osip_remove_transaction ((osip_t *) tr->config, tr);
	OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "psp_core_cb_nist_kill_transaction!\n"));
}

//-----------------------------------------------------------------------------
void
ad_cb_ist_kill_transaction (int type, osip_transaction_t * tr)
{
	static int ist_killed = 0;

	ist_killed++;
	if (ist_killed % 100 == 0)
    {
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_INFO1, NULL,
		   "Number of killed IST transaction = %i\n", ist_killed));
    }

	osip_remove_transaction ((osip_t *) tr->config, tr);

	OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "psp_core_cb_ist_kill_transaction!\n"));
}
//--------------------------------------------------------------------------------
void
ad_cb_nict_kill_transaction (int type, osip_transaction_t * tr)
{
  static int nict_killed = 0;

  nict_killed++;
  if (nict_killed % 100 == 0)
    {
      OSIP_TRACE (osip_trace
                  (__FILE__, __LINE__, OSIP_INFO1, NULL,
                   "Number of killed NICT transaction = %i\n", nict_killed));
    }
  if(tr->ctx_type == NICT &&tr->last_response == NULL )
  {
	  osip_generic_param_t *param ;
	  osip_from_param_get_byname(tr->orig_request->from,"user",&param) ;
	  if(!strcmp(param->gvalue,"stabilize"))
	  mynode->OnRegisterTimeout(tr);
	  if(!strcmp(param->gvalue,"multicast"))
		  mynode->OnDiscoverTimeout(tr);
  }
  osip_remove_transaction ((osip_t *) tr->config, tr);
  OSIP_TRACE (osip_trace
              (__FILE__, __LINE__, OSIP_INFO1, NULL,
               "ad_cb_nict_kill_transaction!\n"));
}
//---------------------------------------------------------------------
void
ad_cb_transport_error (int type, osip_transaction_t * tr, int error)
{
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_ERROR, NULL,
	       "ad_cb_network_error! \n"));
}

//---------------------------------------------------------------------
void
ad_cb_rcv1xx (int type, osip_transaction_t * tr, osip_message_t * sip)
{
	OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcv1xx!\n"));
}

//---------------------------------------------------------------------
void
ad_cb_rcv2xx (int type, osip_transaction_t * tr, osip_message_t * sip)
{
	OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcv2xx!\n"));
	/*reg_t *jr = mynode->reg_find(tr) ;
	if (jr == NULL)		
		OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcv2xx:can't find the reg_t object!\n"));*/
	mynode->OnRegisterSuccess(tr , sip) ;
}

//---------------------------------------------------------------------
void
ad_cb_rcv3xx (int type, osip_transaction_t * tr, osip_message_t * sip)
{
	OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcv3xx!\n"));
	mynode->OnRegisterRedirect(tr , sip) ;
}

//---------------------------------------------------------------------
void
ad_cb_rcv4xx (int type, osip_transaction_t * tr, osip_message_t * sip)
{
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcv4xx!\n"));
}

//---------------------------------------------------------------------
void
ad_cb_rcv5xx (int type, osip_transaction_t * tr, osip_message_t * sip)
{
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcv5xx!\n"));
}

//---------------------------------------------------------------------
void
ad_cb_rcv6xx (int type, osip_transaction_t * tr, osip_message_t * sip)
{
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcv6xx!\n"));
}
//---------------------------------------------------------------------
void
ad_cb_rcvinvite (int type, osip_transaction_t * tr, osip_message_t * sip)
{
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcvinvite!\n"));
}
//---------------------------------------------------------------------
void
ad_cb_rcvack (int type, osip_transaction_t * tr, osip_message_t * sip)
{
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcvack!\n"));
}
//---------------------------------------------------------------------
void
ad_cb_rcvack2 (int type, osip_transaction_t * tr, osip_message_t * sip)
{
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcvack2!\n"));
}
//---------------------------------------------------------------------
/*
find the reg_t object
call the OnRegister()
*/
void
ad_cb_rcvregister (int type, osip_transaction_t * tr, osip_message_t * sip)
{
	OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcvregister!\n"));
		
	mynode->OnRegister(sip,tr) ;
}
//---------------------------------------------------------------------
void
ad_cb_rcvbye (int type, osip_transaction_t * tr, osip_message_t * sip)
{}
void
ad_cb_rcvcancel (int type, osip_transaction_t * tr, osip_message_t * sip)
{}
void
ad_cb_rcvinfo (int type, osip_transaction_t * tr, osip_message_t * sip)
{}
void
ad_cb_rcvoptions (int type, osip_transaction_t * tr, osip_message_t * sip)
{}
void
ad_cb_rcvprack (int type, osip_transaction_t * tr, osip_message_t * sip)
{}
void
ad_cb_rcvunkrequest (int type, osip_transaction_t * tr, osip_message_t * sip)
{
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "ad_cb_rcvunkrequest!\n"));
}
//---------------------------------------------------------------------
void
ad_cb_snd1xx (int type, osip_transaction_t *tr,osip_message_t *sip)
{
  OSIP_TRACE (osip_trace
              (__FILE__, __LINE__, OSIP_INFO1, NULL, "ad_cb_snd1xx!\n"));
}
//---------------------------------------------------------------------
void
ad_cb_snd2xx (int type, osip_transaction_t *tr,osip_message_t *sip)
{
  OSIP_TRACE (osip_trace
              (__FILE__, __LINE__, OSIP_INFO1, NULL, "ad_cb_snd2xx!\n"));
}
//---------------------------------------------------------------------
void
ad_cb_snd3xx (int type, osip_transaction_t *tr,osip_message_t *sip)
{
  OSIP_TRACE (osip_trace
              (__FILE__, __LINE__, OSIP_INFO1, NULL, "ad_cb_snd3xx!\n"));
}
//---------------------------------------------------------------------
void
ad_cb_snd4xx (int type, osip_transaction_t *tr,osip_message_t *sip)
{
  OSIP_TRACE (osip_trace
              (__FILE__, __LINE__, OSIP_INFO1, NULL, "ad_cb_snd4xx!\n"));
}
//---------------------------------------------------------------------
void
ad_cb_snd5xx (int type, osip_transaction_t *tr,osip_message_t *sip)
{
  OSIP_TRACE (osip_trace
              (__FILE__, __LINE__, OSIP_INFO1, NULL, "ad_cb_snd5xx!\n"));
}
//---------------------------------------------------------------------
void
ad_cb_snd6xx (int type, osip_transaction_t *tr,osip_message_t *sip)
{
  OSIP_TRACE (osip_trace
              (__FILE__, __LINE__, OSIP_INFO1, NULL, "ad_cb_snd6xx!\n"));
}
//---------------------------------------------------------------------