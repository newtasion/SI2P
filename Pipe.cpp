/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include <osipparser2/osip_port.h>
#include "pipe.h"

//-----------------------------------------------
#ifndef WIN32

jpipe_t::jpipe_t ()
{
	if (0 != pipe (pipes))
    {
		//error
		return ;
    }
}

jpipe_t::~jpipe_t()
{
	close (apipe->pipes[0]);
	close (apipe->pipes[1]);
}

/**
 * Write in a pipe.
 */
int jpipe_t::jpipe_write (const void *buf, int count)
{
	return write (pipes[1], buf, count);
}

/**
 * Read in a pipe.
 */
int jpipe_t::jpipe_read (void *buf, int count)
{
	return read (pipes[0], buf, count);
}

/**
 * Get descriptor of reading pipe.
 */
int jpipe_t::jpipe_get_read_descr ()
{
	return apipe->pipes[0];
}

#else

jpipe_t::jpipe_t ()
{
	int s = 0;
	int timeout = 0;
	static int aport = 10500;
	struct sockaddr_in raddr;
	int j;

	s = (int) socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (0 > s)
    {
		//error
		return ;
    }
	pipes[1] = (int) socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (0 > pipes[1])
    {
		closesocket(s);
		//error
		return ;
    }

	raddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
	raddr.sin_family = AF_INET;

	j = 50;
	while (aport++ && j-- > 0)//find a good port
    {
		raddr.sin_port = htons ((short) aport);
		if (bind (s, (struct sockaddr *) &raddr, sizeof (raddr)) < 0)
		{
			OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_WARNING, NULL,
				  "Failed to bind one local socket %i!\n",
				  aport));
		}
		else
			break;
    }

	if (j == 0)
    {
		OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
			      "Failed to bind a local socket, aborting!\n"));
		closesocket (s);
		closesocket (pipes[1]);
		return ;
    }

	j = listen(s,1);
	if (j != 0)
    {
		OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
			      "Failed to listen on a local socket, aborting!\n"));
		closesocket(s);
		closesocket(pipes[1]);
		return ;
    }

	j = setsockopt (pipes[1],
		  SOL_SOCKET,
		  SO_RCVTIMEO, (const char*) &timeout, sizeof (timeout));
	if (j != NO_ERROR)
    {
		/* failed for some reason... */
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "udp plugin; cannot set O_NONBLOCK to the file desciptor!\n"));
		closesocket(s);
		closesocket(pipes[1]);
		return ;
    }

	connect (pipes[1], (struct sockaddr *) &raddr, sizeof (raddr));

	pipes[0] = accept (s, NULL, NULL);

	if (pipes[0]<=0)
    {
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "udp plugin; Failed to call accept!\n"));
		closesocket(s);
		closesocket(pipes[1]);
		return ;
    }
}

jpipe_t::~jpipe_t ()
{
	closesocket (pipes[0]);
	closesocket (pipes[1]);
}


/**
 * Write in a pipe.
 */
int jpipe_t::jpipe_write (const void *buf, int count)
{
	return send (pipes[1], (const char *)buf, count, 0);
}

/**
 * Read in a pipe.
 */
int jpipe_t::jpipe_read (void *buf, int count)
{
	return recv (pipes[0], (char *)buf, count, 0 /* MSG_DONTWAIT */ );	/* BUG?? */
}

/**
 * Get descriptor of reading pipe.
 */
int jpipe_t::jpipe_get_read_descr ()
{
	return pipes[0];
}

#endif