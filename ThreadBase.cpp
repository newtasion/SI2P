#ifndef OSIP_MT
#define OSIP_MT
#endif
#include <osip2/osip_mt.h>
#include <osip2/osip_condv.h>
#include <osip2/osip.h>

#include "baseunit.h"
#include "ThreadBase.h"

//------------------------------------------------------------------
ThreadBase::ThreadBase():wakeup()
{
	thread = NULL ;
}
/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

//------------------------------------------------------------------
ThreadBase::~ThreadBase()
{
	if (thread != NULL)
    {
		ThreadRelease() ;
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_BUG, NULL,
		   "You must call module_release() before calling module_free()!\n"));
		return;
    }
}

//------------------------------------------------------------------
int ThreadBase::ThreadRelease() 
{
	char q[2] = "q";
	int i;

	if (thread == NULL)
		return 0;                   /* already stopped? */

	i = wakeup.jpipe_write (&q, 1);
	if (i != 1)
    {
		OSIP_TRACE (osip_trace
                  (__FILE__, __LINE__, OSIP_ERROR, NULL,
                   "could not write in pipe!\n"));
		return -1;
    }

	i = osip_thread_join ((struct osip_thread *)thread);

	if (i != 0)
    {
		OSIP_TRACE (osip_trace
                  (__FILE__, __LINE__, OSIP_ERROR, NULL,
                   "could not shutdown thread!\n"));
		return -1;
    }
	osip_free (thread);
	thread = NULL;
	return 0;
}

//------------------------------------------------------------------
int ThreadBase::ThreadStart(void *(*func_start) (void *), void *arg) 			//start the thread
{
	if (func_start == NULL)
		return -1;
    thread = (void *)osip_thread_create (20000, func_start, arg);
    if (thread == NULL)
		return -1;
    return 0;
}

//------------------------------------------------------------------
int ThreadBase::ThreadWakeUp ()
{
	int i;
	char q[2] = "w";

	i = wakeup.jpipe_write (&q, 1);
	if (i != 1)
    {
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "could not write in pipe!\n"));
		perror ("error while writing in pipe");
		return -1;
    }
	return 0;
}



