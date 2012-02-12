/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define snprintf _snprintf
#define close(s) closesocket(s)
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

/***-----------WHEN UNDER WINDOWS----------***/
#ifdef WIN32
//when underwindows
#include <Windows.h>
#include <Iphlpapi.h>
#include <MALLOC.h>

#include <osipparser2/osip_port.h>
#include "pubtool.h"
//------------------------------------------------------------------------
int
pub_dns_get_local_fqdn (char **servername, char **serverip,
			char **netmask, unsigned int WIN32_interface)
{
	unsigned int pos;
	*servername = NULL; /* no name on win32? */
	*serverip   = NULL;
	*netmask    = NULL;

	/* First, try to get the interface where we should listen */
	{
		DWORD size_of_iptable = 0;
		PMIB_IPADDRTABLE ipt;
		PMIB_IFROW ifrow;

		if (GetIpAddrTable(NULL, &size_of_iptable, TRUE) == ERROR_INSUFFICIENT_BUFFER)
		{
			ifrow = (PMIB_IFROW) _alloca (sizeof(MIB_IFROW));
			ipt = (PMIB_IPADDRTABLE) _alloca (size_of_iptable);
			if (ifrow==NULL || ipt==NULL)
			{
				/* not very usefull to continue */
				OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_INFO4, NULL,
					"ERROR alloca failed\r\n"));
				return -1;
			}

			if (!GetIpAddrTable(ipt, &size_of_iptable, TRUE))
			{
				/* look for the best public interface */

				for (pos=0; pos < ipt->dwNumEntries && *netmask==NULL ; ++pos)
				{
					/* index is */
					struct in_addr addr;
					struct in_addr mask;
					ifrow->dwIndex = ipt->table[pos].dwIndex;
					if (GetIfEntry(ifrow) == NO_ERROR)
					{
						switch(ifrow->dwType)
						{
						case MIB_IF_TYPE_LOOPBACK:
						  /*	break; */
						case MIB_IF_TYPE_ETHERNET:
						default:
							addr.s_addr = ipt->table[pos].dwAddr;
							mask.s_addr = ipt->table[pos].dwMask;
							if (ipt->table[pos].dwIndex == WIN32_interface)
							{
								*servername = NULL; /* no name on win32? */
								*serverip   = osip_strdup(inet_ntoa(addr));
								*netmask    = osip_strdup(inet_ntoa(mask));
								OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_INFO4, NULL,
									"Interface ethernet: %s/%s\r\n", *serverip, *netmask));
								break;
							}
						}
					}
				}
			}
		}
	}

	if (*serverip==NULL || *netmask==NULL)
	{
		OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_WARNING, NULL,
			"ERROR No network interface found\r\n"));
		return -1;
	}

	return 0;
}

//------------------------------------------------------------------------

int
pub_guess_ip_for_via (int family, char *address, int size)
{
	/* w2000 and W95/98 */
	unsigned long  best_interface_index;
	DWORD hr;

	/* NT4 (sp4 only?) */
	PMIB_IPFORWARDTABLE ipfwdt;
	DWORD siz_ipfwd_table = 0;
	unsigned int ipf_cnt;

	address[0] = '\0';
	best_interface_index = -1;
	/* w2000 and W95/98 only */
	hr = GetBestInterface(inet_addr("217.12.3.11"),&best_interface_index);
	if (hr)
	{
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR) &lpMsgBuf, 0, NULL);

		OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_INFO4, NULL,
					 "GetBestInterface: %s\r\n", lpMsgBuf));
		best_interface_index = -1;
	}

	if (best_interface_index != -1)
	{ /* probably W2000 or W95/W98 */
		char *servername;
		char *serverip;
		char *netmask;
		OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_INFO4, NULL,
					 "Default Interface found %i\r\n", best_interface_index));

		if (0 == pub_dns_get_local_fqdn(&servername, &serverip, &netmask,
						best_interface_index))
		{
			osip_strncpy(address, serverip, size);
			osip_free(servername);
			osip_free(serverip);
			osip_free(netmask);
			return 0;
		}
		return -1;
	}


	if (!GetIpForwardTable(NULL, &siz_ipfwd_table, FALSE) == ERROR_INSUFFICIENT_BUFFER
		|| !(ipfwdt = (PMIB_IPFORWARDTABLE) alloca (siz_ipfwd_table)))
	{
		OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_INFO4, NULL,
			"Allocation error\r\n"));
		return -1;
	}


	/* NT4 (sp4 support only?) */
	if (!GetIpForwardTable(ipfwdt, &siz_ipfwd_table, FALSE))
	{
		for (ipf_cnt = 0; ipf_cnt < ipfwdt->dwNumEntries; ++ipf_cnt) 
		{
			if (ipfwdt->table[ipf_cnt].dwForwardDest == 0)
			{ /* default gateway found */
				char *servername;
				char *serverip;
				char *netmask;
				OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_INFO4, NULL,
					"Default Interface found %i\r\n", ipfwdt->table[ipf_cnt].dwForwardIfIndex));

				if (0 ==  pub_dns_get_local_fqdn(&servername,
								 &serverip,
								 &netmask,
								 ipfwdt->table[ipf_cnt].dwForwardIfIndex))
				{
					osip_strncpy(address, serverip, size);
					osip_free(servername);
					osip_free(serverip);
					osip_free(netmask);
					return 0;
				}
				return -1;
			}
		}

	}
	/* no default gateway interface found */
	return -1;
}



/*-----------WHEN UNDER POSIX----------*/
/* sun, *BSD, linux, and other? */
#else 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/param.h>

#include <stdio.h>

static int pub_dns_default_gateway_ipv4 (char *address, int size);

int
pub_guess_ip_for_via (int familiy, char *address, int size)
{
	return pub_dns_default_gateway_ipv4 (address, size);
}

/* This is a portable way to find the default gateway.
 * The ip of the default interface is returned.
 */
static int
pub_dns_default_gateway_ipv4 (char *address, int size)
{
  unsigned int len;
  int sock_rt, on=1;
  struct sockaddr_in iface_out;
  struct sockaddr_in remote;
  
  memset(&remote, 0, sizeof(struct sockaddr_in));

  remote.sin_family = AF_INET;
  remote.sin_addr.s_addr = inet_addr("217.12.3.11");
  remote.sin_port = htons(11111);
  
  memset(&iface_out, 0, sizeof(iface_out));
  sock_rt = socket(AF_INET, SOCK_DGRAM, 0 );
  
  if (setsockopt(sock_rt, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on))
      == -1) {
    perror("DEBUG: [get_output_if] setsockopt(SOL_SOCKET, SO_BROADCAST");
    close(sock_rt);
    return -1;
  }
  
  if (connect(sock_rt, (struct sockaddr*)&remote, sizeof(struct sockaddr_in))
      == -1 ) {
    perror("DEBUG: [get_output_if] connect");
    close(sock_rt);
    return -1;
  }
  
  len = sizeof(iface_out);
  if (getsockname(sock_rt, (struct sockaddr *)&iface_out, &len) == -1 ) {
    perror("DEBUG: [get_output_if] getsockname");
    close(sock_rt);
    return -1;
  }

  close(sock_rt);
  if (iface_out.sin_addr.s_addr == 0)
    { /* what is this case?? */
      return -1;
    }
  osip_strncpy(address, inet_ntoa(iface_out.sin_addr), size-1);
  return 0;
}
#endif



//----common--------
int 
pub_guess_localip(int family, char *address, int size)
{
	return pub_guess_ip_for_via (family, address, size) ;	
}
//------------------------------------------------------------------------

char *strdup_printf(const char *fmt, ...)
{
	/* Guess we need no more than 100 bytes. */
	int n, size = 100;
	char *p;
	va_list ap;
	if ((p = (char *)osip_malloc (size)) == NULL)
		return NULL;
	while (1)
	{
		/* Try to print in the allocated space. */
		va_start (ap, fmt);
#ifdef WIN32
		n = _vsnprintf (p, size, fmt, ap);
#else
		n = vsnprintf (p, size, fmt, ap);
#endif
		va_end (ap);
		/* If that worked, return the string. */
		if (n > -1 && n < size)
			return p;
		/* Else try again with more space. */
		if (n > -1)	/* glibc 2.1 */
			size = n + 1;	/* precisely what is needed */
		else		/* glibc 2.0 */
			size *= 2;	/* twice the old size */
		if ((p = (char *)realloc (p, size)) == NULL)
			return NULL;
	}
}

//-------------------------------------------------------------------------------
unsigned int
via_branch_new_random(void)
{
  return osip_build_random_number();
}

//-------------------------------------------------------------------------------
char * pub_inet_ntop (struct sockaddr *sockaddr)
{
  char *ptr = NULL;
  switch (sockaddr->sa_family) {
  case AF_INET:
	  ptr = inet_ntoa (((struct sockaddr_in *)sockaddr)->sin_addr);
	  break;
  case AF_INET6:
	  ptr = NULL;
      break;
  default:
    return NULL;
  }
  if (ptr==NULL)
    return NULL;
  return osip_strdup(ptr);
}

//-------------------------------------------------------------------------------
int 
pub_get_addrinfo (struct addrinfo **addrinfo, char *hostname, int service)
{
	unsigned long int one_inet_addr;
	struct addrinfo hints;
	int error;
	char portbuf[10];
	if (service!=0)
		_snprintf(portbuf, sizeof(portbuf), "%d", service);

	memset (&hints, 0, sizeof (hints));
	if ((int) (one_inet_addr = inet_addr (hostname)) == -1)
		hints.ai_flags = AI_CANONNAME;
	else
    {
		#if 0
			struct addrinfo *_addrinfo;
			_addrinfo = (struct addrinfo *)osip_malloc(sizeof(struct addrinfo)
					     + sizeof (struct sockaddr_in)
					     + 0); /* no cannonname */
			memset(_addrinfo, 0, sizeof(struct addrinfo) + sizeof (struct sockaddr_in) + 0);
			_addrinfo->ai_flags = AI_NUMERICHOST;
			_addrinfo->ai_family = AF_INET;
			_addrinfo->ai_socktype = SOCK_DGRAM;
			_addrinfo->ai_protocol = IPPROTO_UDP;
			_addrinfo->ai_addrlen = sizeof(struct sockaddr_in);
			_addrinfo->ai_addr = (void *) ((_addrinfo) + sizeof (struct addrinfo));

			((struct sockaddr_in*)_addrinfo->ai_addr)->sin_family = AF_INET;
			((struct sockaddr_in*)_addrinfo->ai_addr)->sin_addr.s_addr = one_inet_addr;
			
			if (service==0)
				((struct sockaddr_in*)_addrinfo->ai_addr)->sin_port   = htons (DEFAULTPORT);
			else
				((struct sockaddr_in*)_addrinfo->ai_addr)->sin_port   = htons ((unsigned short)service);
			_addrinfo->ai_canonname = NULL;
			_addrinfo->ai_next = NULL;

			OSIP_TRACE (osip_trace
			  (__FILE__, __LINE__, OSIP_INFO2, NULL,
			   "No DNS resolution needed for %s:%i\n", hostname, service));
			*addrinfo = _addrinfo;
			return 0;
		
		#else
			hints.ai_flags = AI_NUMERICHOST;
		#endif
	}

	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	if (service==0)
    {
		error = getaddrinfo (hostname, "sip", &hints, addrinfo);
		OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_INFO2, NULL,
		   "SRV resolution with udp-sip-%s\n", hostname));
    }
	else
    {
		error = getaddrinfo (hostname, portbuf, &hints, addrinfo);
		OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_INFO2, NULL,
		   "DNS resolution with %s:%i\n", hostname, service));
    }

	if (error || *addrinfo == NULL)
    { 
		OSIP_TRACE (osip_trace(__FILE__, __LINE__, OSIP_INFO2, NULL,
		   "getaddrinfo failure. %s:%i\n", hostname, service));
		return -1;
    }
	return 0;
}

unsigned int uhash(char * src)
{	
	unsigned int hash = 0;
	unsigned int x    = 0;

    for(char *p = src; p < src + strlen(src); p++)
    {
		hash = (hash << 4) + *p;
        if((x = hash & 0xF0000000L) != 0)
        {
                hash ^= (x >> 24);
                hash &= ~x;
        }
    }

    return (hash & 0x1F);
}