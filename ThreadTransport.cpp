/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * 
 */
//0826

#include <ERRNO.h>
#include <windowsx.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Iphlpapi.h>

#include "pubtool.h"
#include "pipe.h"
#include "ThreadTransport.h"



//----------------------------------------------------------------------------------
ThreadTransport::ThreadTransport(Node *node)
{
	int atry ;
	int option =1 ;
	int i = 0 ;
	admin_core = node ;

	/* init socket */

	struct sockaddr_in raddr;

	/*in_port 和 out_port 暂时设置为localport */
	int locport = atoi(node->localport) ;
	in_port = locport ;
	out_port = in_port ;
	//----------------

	/* in_socket */
	in_socket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (in_socket == -1)
    {
		OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
			      "udp plugin: cannot create descriptor for port %i!\n",
			      in_port));
		goto tr_error1;
    }

	/*Allows the socket to be bound to an address that is already in use. */
	option = 1;
	i = setsockopt (in_socket, SOL_SOCKET, SO_REUSEADDR,
		  (char *)&option, sizeof option);
	if (i!=0)
    {
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_WARNING, NULL,
		   "upd plugin; UDP listener SO_REUSE_ADDR failed %i (%i)!\n",
		   in_port, i));
    }

	
	if (out_port == in_port)
		out_socket = in_socket;
	else
    {
		/* out_socket */
		out_socket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (out_socket == -1)
		{
			OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
				  "udp plugin: cannot create descriptor for port %i!\n",
				  out_port));
			goto tr_error2;
		}

		/* try out_port */
		i = (-1);
		atry = 0;
		while (i < 0 && atry < 40)
		{
			atry++;
			{
				/* bind out_sock */
				raddr.sin_addr.s_addr = htons (INADDR_ANY);
				raddr.sin_port = htons ((short) out_port);
				raddr.sin_family = AF_INET;
				i = bind (out_socket,
				   (struct sockaddr *) &raddr, sizeof (raddr));
			}

			if (i < 0)
			{
				OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_WARNING, NULL,
			   "udp plugin: cannot bind on port %i!\n",out_port));
				out_port++;
			}
		}
		//----

		if (i != 0)
			goto tr_error3;
    }

	/* bind in_sock */
	raddr.sin_addr.s_addr = htons (INADDR_ANY);
	raddr.sin_port = htons ((short) in_port);
	raddr.sin_family = AF_INET;
	i = bind (in_socket,(struct sockaddr *) &raddr, sizeof (raddr));
  
	if (i < 0)
    {
		OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_WARNING, NULL,
		   "udp plugin: cannot bind on port %i!\n",in_port));
		goto tr_error2;
    }

	
	//-------------------------
	/* prepare for mutilcast */
	struct ip_mreq mreq;
	
	#ifdef __linux
      /* linux has support for listening on specific interface. */
    {
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	}

	#else /* non __linux */
	{
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	}
	#endif /* __linux */

	/* For IPv4, sip.mcast.org is the well-known IPv4 multicast IP address*/
	/* 224.0.1.75 is the multicast IP address of sip.mcast.org */
	mreq.imr_multiaddr.s_addr = inet_addr("224.0.1.75");
  
	if (in_port== DEFAULTPORT)
		mcast_socket = in_socket; /* reuse the same socket */
	else
	{ /* open a new socket on DEFAULTPORT for multicast */
		raddr.sin_addr.s_addr = htons (INADDR_ANY);
		raddr.sin_port = htons ((short) DEFAULTPORT); /* always DEFAULTPORT */
		raddr.sin_family = AF_INET;
	  
		mcast_socket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (mcast_socket<0)
		{
			OSIP_TRACE (osip_trace
			  (__FILE__, __LINE__, OSIP_WARNING, NULL,
			   "upd plugin; Cannot create socket for multicast!\n"));
			mcast_socket = -1;
			goto skip_multicast;
		}
	  
		option = 1;
		i = setsockopt (mcast_socket, SOL_SOCKET, SO_REUSEADDR,
			  (char *) &option, sizeof option);
		if (i!=0)
		{
			OSIP_TRACE (osip_trace
			  (__FILE__, __LINE__, OSIP_WARNING, NULL,
			   "upd plugin; UDP listener SO_REUSE_ADDR failed %i (%i)!\n",
			   DEFAULTPORT, i));
			closesocket(mcast_socket);
			mcast_socket = -1;
			goto skip_multicast;
		}
	  
		i = bind (mcast_socket,(struct sockaddr *) &raddr, sizeof (raddr));
		if (i < 0)
		{
			OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_WARNING, NULL,
			   "udp plugin: disabling multicast support %i (%s)!\n",
			   DEFAULTPORT, strerror (errno)));
			closesocket(mcast_socket);
			mcast_socket = -1;
			goto skip_multicast;
		}
	}
  
	option=0;
	if(0 != setsockopt(mcast_socket, IPPROTO_IP, IP_MULTICAST_LOOP,
			 (char *) &option, sizeof(option)))
	{
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_WARNING, NULL,"udp plugin; cannot disable loopback for multicast socket. (%i)!\n",
		   5061));
		if (in_port!=DEFAULTPORT)
			closesocket(mcast_socket);
		mcast_socket = -1;
		goto skip_multicast;
	}
  
	/* Enable Multicast support */
	option=1;
	if (0 != setsockopt(mcast_socket, IPPROTO_IP, IP_MULTICAST_TTL,
			  (char *) &option, sizeof(option)))
	{
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_WARNING, NULL,
		   "udp plugin: cannot set multicast ttl value. (%i)!\n",DEFAULTPORT));
		if (in_port!=DEFAULTPORT)
			closesocket(mcast_socket);
		mcast_socket = -1;
		goto skip_multicast;
	}
  
	if (0 != setsockopt (mcast_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			   (char *) &mreq, sizeof(mreq)))
	{
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_WARNING, NULL,
		   "udp plugin; cannot set multicast ttl value. (%i)!\n",
		   DEFAULTPORT));
		if (in_port!=DEFAULTPORT)
			closesocket(mcast_socket);
		mcast_socket = -1;
		goto skip_multicast;
	}

    skip_multicast:

	/* set the socket to never block on recv() calls */
	#ifndef WIN32
		if (0 != fcntl (in_socket, F_SETFL, O_NONBLOCK))
		{
			OSIP_TRACE (osip_trace
			  (__FILE__, __LINE__, OSIP_ERROR, NULL,
			   "udp plugin; cannot set O_NONBLOCK to the file desciptor (%i)!\n",
			   in_port));
			goto tr_error5;
		}
		if (0 != fcntl (out_socket, F_SETFL, O_NONBLOCK))
		{
			OSIP_TRACE (osip_trace
			  (__FILE__, __LINE__, OSIP_ERROR, NULL,
			   "udp plugin; cannot set O_NONBLOCK to the file desciptor (%i)!\n",
			   out_port));
			goto tr_error5;
		}

	#else
	{
		int timeout = 0;
		int err;

		err = setsockopt (in_socket,SOL_SOCKET,
		      SO_RCVTIMEO, (char *) &timeout, sizeof (timeout));
		if (err != NO_ERROR)
		{
			/* failed for some reason... */
			OSIP_TRACE (osip_trace
		    (__FILE__, __LINE__, OSIP_ERROR, NULL,
			"udp plugin; cannot set O_NONBLOCK to the file desciptor (%i)!\n",
		     in_port));
			goto tr_error5;
		}
		err = setsockopt (out_socket,
		      SOL_SOCKET,
		      SO_RCVTIMEO, (char *) &timeout, sizeof (timeout));
		if (err != NO_ERROR)
		{
			/* failed for some reason... */
			OSIP_TRACE (osip_trace
		    (__FILE__, __LINE__, OSIP_ERROR, NULL,
		     "udp plugin; cannot set O_NONBLOCK to the file desciptor (%i)!\n",
		     out_port));
			goto tr_error5;
		}
	}
	#endif

	return;

tr_error5:
tr_error3:
	closesocket (out_socket);
tr_error2:
	closesocket (in_socket);
tr_error1:
	return ;
}
//----------------------------------------------------------------------------------
ThreadTransport::~ThreadTransport()
{
	if (in_socket != -1)
    {
		closesocket (in_socket);
		in_socket = -1;
    }
	if (in_port == out_port)
		out_socket = (-1);
	else if (out_socket != -1)
		closesocket (out_socket);
}

/*  */
int ThreadTransport::Transport_execute(int sec_max, int usec_max, int max_analysed)
{
	
	fd_set tlp_fdset;
	int max_fd;
	struct timeval tv;

	while (1)
    {
		int i;
		int s;

		tv.tv_sec = sec_max;
		tv.tv_usec = usec_max;

		max_analysed--;
		FD_ZERO (&tlp_fdset);
		s = wakeup.jpipe_get_read_descr() ;
		if (s <= 1)
		{
			OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_ERROR, NULL,
		       "No wakeup value in module tlp!\n"));
			return -1;
		}
		FD_SET (s, &tlp_fdset);

		max_fd = s;
		/* add all descriptor in fdset, so any incoming message will wake up this thread */
		if (in_socket > 0)
		{
			if (max_fd < in_socket)
				max_fd = in_socket;
			FD_SET (in_socket, &tlp_fdset);
			if (in_socket!=out_socket && out_socket>0)
			{
				FD_SET (out_socket, &tlp_fdset);
				if (max_fd<out_socket)
					max_fd = out_socket;
			}
			if (in_socket!=mcast_socket && mcast_socket>0)
			{
				FD_SET (mcast_socket, &tlp_fdset);
				if (max_fd<mcast_socket)
					max_fd = mcast_socket;
			}
		}
	
		//select to listen
		if ((sec_max == -1) || (usec_max == -1))
			i = select (max_fd + 1, &tlp_fdset, NULL, NULL, NULL);
	    else
			i = select (max_fd + 1, &tlp_fdset, NULL, NULL, &tv);

		if (FD_ISSET (s, &tlp_fdset))
		{			
			char tmp[2];
			i = wakeup.jpipe_read(tmp, 1);
			if (i == 1 && tmp[0] == 'q')
			{
				OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_INFO2, NULL,
				"Exiting transport module!\n"));
				return 0;
			}
		}

		if (in_socket > 0)
		{
			if (FD_ISSET (in_socket, &tlp_fdset))
				tran_rcv_message(1);
			else if (out_socket>0 && FD_ISSET (out_socket, &tlp_fdset))
				tran_rcv_message (1);
			else if (mcast_socket>0 && FD_ISSET (mcast_socket, &tlp_fdset))
				tran_rcv_message (1);
		}
		else
			tran_rcv_message (5);
		
		if (max_analysed == 0)
			return 0 ;
	}
}

//----------------------------------------------------------------------------------
/* This method returns:
   -1 on error
   0  on no message available
   1  on max_analysed reached
*/
int 
ThreadTransport::tran_rcv_message (int max)
{
	fd_set memo_fdset;
	fd_set osip_fdset;
	int max_fd;
	struct sockaddr_in sa4;
	struct sockaddr *sa;
	char *buf;
	int i;
	struct timeval tv;

	#ifdef __linux
		socklen_t slen;
	#else
		int slen;
	#endif

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO (&memo_fdset);
	FD_SET (in_socket, &memo_fdset);
	if (out_socket>0 &&out_socket!=in_socket)
		FD_SET (out_socket, &memo_fdset);
	if (mcast_socket>0 &&mcast_socket!=in_socket)
		FD_SET (mcast_socket, &memo_fdset);

	for(; max != 0; max--)
	{
		osip_fdset = memo_fdset;
		max_fd = in_socket;
		if (max_fd < out_socket)
			max_fd = out_socket;
		if (max_fd < mcast_socket)
			max_fd = mcast_socket;

		//listen...
		i = select (max_fd + 1, &osip_fdset, NULL, NULL, NULL);
		buf = (char *) osip_malloc (SIP_MESSAGE_MAX_LENGTH * sizeof (char) + 3);
		slen = sizeof (sa4);
		sa = (struct sockaddr *)&sa4;

		if (0 == i)
		{
			osip_free (buf);
			return -1;		/* no message: timout expires */
		}
		else if (-1 == i)
		{
			osip_free (buf);
			return -2;		/* error */
		}
		else if (FD_ISSET (in_socket, &osip_fdset))
		{
			OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_INFO2, NULL,
			   "UDP MESSAGE RECEIVED\n"));
			i = recvfrom (in_socket, buf, SIP_MESSAGE_MAX_LENGTH, 0,
				   sa, &slen);
		}
		else if (out_socket!=in_socket &&FD_ISSET (out_socket, &osip_fdset))
		{
			OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_INFO2, NULL,
			   "UDP MESSAGE RECEIVED\n"));
			i = recvfrom (out_socket, buf, SIP_MESSAGE_MAX_LENGTH, 0,
				   sa, &slen);
		}
		else if (mcast_socket>0 &&FD_ISSET (mcast_socket, &osip_fdset))
		{
			OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_INFO2, NULL,
			   "UDP MULTICAST MESSAGE RECEIVED\n"));
			i =recvfrom (mcast_socket, buf, SIP_MESSAGE_MAX_LENGTH, 0,
				 sa, &slen);
		}

		if (i > 0)
		{
			//char *ip_address = ppl_inet_ntop (((struct sockaddr_in *)sa)->sin_addr);
			char *ip_address = pub_inet_ntop (sa);
			/* i is the length of buf ,but must add "\0" at the end of buf */
			osip_strncpy (buf + i, "\0", 1);
			if (ip_address==NULL)
			{
				osip_free(buf);
				return -1; /* missing information from socket?? */
			}
			OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_INFO2, NULL,
				   "udp plugin: RCV UDP MESSAGE (from %s:%i)\n",
				   /* inet_ntoa (sa.sin_addr), ntohs (sa.sin_port))); */
				   ip_address, ntohs (sa4.sin_port)));
			if (mcast_socket>0)
			{
			  /* check maddr parameter to detect multicast data */
			  /* do they need special processing? */
			}

			OSIP_TRACE (osip_trace
				  (__FILE__, __LINE__, OSIP_INFO1, NULL, "\n%s\n", buf));

			/* start process message */
			//printf("RECEIVE<<======================================\n") ;
			//printf(buf) ;
			tran_process_message (buf, ip_address, ntohs (sa4.sin_port), i);
			osip_free(ip_address);
		}
		else if (i == -1)
		{
			if (errno == EAGAIN)
			{
				osip_free (buf);
				return 0;
			}
			OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_ERROR, NULL,
				   "udp plugin: error while receiving data!\n"));
			osip_free (buf);
			return -1;
		}
	}
	/* max is reached */
	return 1;
}


//----------------------------------------------------------------------------------
int 
ThreadTransport::tran_process_message (char *buf, char *ip_addr, int port, int length) 
{
	int i ;
	osip_event_t *evt;
	osip_transaction_t *transaction;

	/* buf的基本检查 */
	if (buf == NULL
		|| *buf == '\0'
		|| buf[1] == '\0'
		|| buf[2] == '\0' || buf[3] == '\0' || buf[4] == '\0' || buf[5] == '\0')
    {
		osip_free (buf);
		return -1;
    }

	evt = osip_parse (buf, length);
	osip_free (buf);
	
	/* 如果不能解析就丢弃 */
	if (evt == NULL)
    {
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "transport module: Could not parse response!\n"));
		 return -1;
    }
	if (evt->sip == NULL)
    {
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "udp module: Could not parse response!\n"));
		osip_event_free (evt);
		return -1;
    }

	/* modify the request to add a "received" parameter in the last Via. */
	/* for libosip>0.9.2 use osip_message_fix_last_via_header from osip */
	__osip_message_fix_last_via_header (evt->sip, ip_addr, port);

	
	/*分发消息*/

	/* 如果是请求*/
	if(MSG_IS_REGISTER(evt->sip))
	{
		i = osip_find_transaction_and_add_event (mynode->adosip->j_osip, evt);
		
		//如果找到对应的事务
		if (i == 0)
		{
			mynode->adosip->ThreadWakeUp ();	/*evt consumed */
			return 0;			
		}
		/* 这个事件没有对应的事务 */
		else
		{
			//创建事务
			transaction = osip_create_transaction (mynode->adosip->j_osip, evt) ;
			if (transaction == NULL)	//创建失败
			{
				OSIP_TRACE (osip_trace
                      (__FILE__, __LINE__, OSIP_INFO3, NULL,
                       "core module: Could not create a transaction for this request!\n"));
				osip_event_free (evt);
				return -1;
			}
			//向事务中添加evt
			osip_transaction_add_event (transaction, evt);
			
			//唤醒osip
			mynode->adosip->ThreadWakeUp() ;
			return 0 ;
		}
	}

	/*如果是响应*/
	//else if(MSG_IS_RESPONSE_FOR(evt->sip, "REGISTER"))
	else if(evt->sip->status_code!=0&&0==strcmp(evt->sip->cseq->method,"REGISTER"))
	{
		i = osip_find_transaction_and_add_event (mynode->adosip->j_osip, evt);
		
		//如果找到对应的事务
		if (i == 0)
		{
			mynode->adosip->ThreadWakeUp ();	/*evt consumed */
			return 0;			
		}
		/* 这个事件没有对应的事务 */
		else
		{
			//什么也不做
			return 0 ;
		}	
	}

	return 0;
}

//----------------------------------------------------------------------------------
int
ThreadTransport::__osip_message_fix_last_via_header (osip_message_t * request, char *ip_addr, int port)
{
	osip_generic_param_t *rport;
	osip_via_t *via;
	/* get Top most Via header: */
	if (request == NULL || request == NULL)
		return -1;
	if (MSG_IS_RESPONSE (request))
		return 0;			/* Don't fix Via header */

	via = (osip_via_t *)osip_list_get (request->vias, 0);
	if (via == NULL || via->host == NULL)
		return -1;

	osip_via_param_get_byname (via, "rport", &rport);

	/* detect rport */
	if (rport != NULL)
    {
		if (rport->gvalue == NULL)
		{
			rport->gvalue = (char *) osip_malloc (9);
			#ifdef WIN32
				_snprintf (rport->gvalue, 8, "%i", port);
			#else
				snprintf (rport->gvalue, 8, "%i", port);
			#endif
		}			/* else bug? */
    }

	/* only add the received parameter if the 'sent-by' value does not contains
     this ip address */
	if (0 == osip_strcasecmp (via->host, ip_addr))	/* don't need the received parameter */
		return 0;
	else
		osip_via_set_received (via, osip_strdup (ip_addr));
	return 0;
}


//-----------------------------------------------------------------------------------
int
ThreadTransport::tran_snd_message (osip_transaction_t * transaction,	/* read only element */
		    osip_message_t * message,	/* message to send           */
		    char *host,	/* proposed destination host */
		    int port,	/* proposed destination port */
		    int socket)	/* proposed socket (if any)  */
{
	size_t length;
	int i;
	char *buf;
	int sock;
	struct addrinfo *addrinfo;

	i = osip_message_to_str (message, &buf, &length);

	if (i != 0)
		return -1;

	/* For RESPONSE, oSIP ALWAYS provide host and port from the top via */
	/* For REQUEST,  oSIP SOMETIMES provide host and port to use which
     may be different from the request uri */

	
	if (host == NULL)
	{				/* when host is NULL, we use the request uri value */
		host = message->req_uri->host;
		if (message->req_uri->port != NULL)
			port = osip_atoi (message->req_uri->port);
		else
			port = DEFAULTPORT;
	}
	
	i = pub_get_addrinfo(&addrinfo, host, port);
	if (i!=0)
    {
		osip_free (buf);
		return -1;
    }
	#if 0
		memcpy (&addr, addrinfo->ai_addr, addrinfo->ai_addrlen);
		freeaddrinfo (addrinfo);
	#endif

	sock = out_socket;

	//printf("SEND======================================>>\n") ;
	//printf(buf) ;
	i = sendto (sock, buf, length, 0,
		addrinfo->ai_addr, addrinfo->ai_addrlen);
  
	freeaddrinfo (addrinfo);

	if (0 > i)
    {
		osip_free (buf);
		#ifdef WIN32
			i = WSAGetLastError ();
			if (WSAECONNREFUSED == i)
			{
				OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
				  "SIP_ECONNREFUSED - No remote server.\n"));
				return 1;		/* I prefer to answer 1 by now..
				   we'll see later what's better */
			}
		#else
			i = errno;
			if (ECONNREFUSED == i)
			{
				OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
				  "SIP_ECONNREFUSED - No remote server.\n"));
				return 1;		/* I prefer to answer 1 by now..
				   we'll see later what's better */
			}
		#endif

		#ifdef WIN32
			OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
			      "SIP_NETWORK_ERROR - Network error %i sending message to %s on port %i.\n",
			      i, host, port));
		#else
			/* XXX use some configure magic to detect if we have the right strerror_r()
			* function (the one that returns int, not char *) and then use strerror_r()
			* instead.
			*/
			OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
			      "SIP_NETWORK_ERROR - Network error %i (%s) sending message to %s on port %i.\n",
			      i, strerror (i), host, port));
		#endif
		/* SIP_NETWORK_ERROR; */
		return -1;
    }

	#ifdef WIN32
		i = WSAGetLastError ();
		if (WSAECONNREFUSED == i)
		{
			OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
			      "SIP_ECONNREFUSED - No remote server.\n"));
			return 1;
		}
		if (WSAECONNRESET == i)
		{
			OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
			      "SIP_ECONNREFUSED - No remote server.\n"));
			return 1;
		}
		OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
			  "WSAGetLastError returned : %i.\n", i));
	#endif

	OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_INFO1, NULL,
	       "udp_plugin: message sent to %s on port %i\n", host, port));

	osip_free (buf);
	return 0;
}



