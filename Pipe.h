/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef _PIPE_H_
#define _PIPE_H_

#ifndef WIN32
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif

#ifdef WIN32

#include <winsock.h>
#endif

class jpipe_t
{
private:
	int pipes[2]; 
public:
	/** * Get New pipe pair. */
    jpipe_t (void);

	/** * Close pipe */
    ~jpipe_t ();

	/** * Write in a pipe. */
    int jpipe_write (const void *buf,
				      int count);

	/** * Read in a pipe. */
    int jpipe_read (void *buf,
				     int count);

	/** * Get descriptor of reading pipe. */
    int jpipe_get_read_descr ();
} ;




#endif