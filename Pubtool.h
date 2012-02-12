/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef __PUBTOOL_H__
#define __PUBTOOL_H__

#ifndef OSIP_MT
	#define OSIP_MT
#endif

//-------------------------------------------------------------------------
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(x)	if( (x)!=NULL ) { free(x); (x)=NULL; }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)	if( (x)!=NULL ) { (x)->Release(); (x)=NULL; }
#endif

//-------------------------------------------------------------------------
#define REMOVE_ELEMENT(first_element, element)				\
	if (element->parent==NULL)								\
	{														\
		first_element = element->next;						\
        if (first_element!=NULL)							\
			first_element->parent = NULL;					\
		}													\
		else												\
        {													\
			element->parent->next = element->next;			\
			if (element->next!=NULL)						\
				element->next->parent = element->parent;	\
				element->next = NULL;						\
				element->parent = NULL;						\
		}

#define ADD_ELEMENT(first_element, element)					\
	if (first_element==NULL)									\
	{														\
		first_element   = element;							\
		element->next   = NULL;								\
		element->parent = NULL;								\
	}														\
	else														\
    {														\
		element->next   = first_element;						\
		element->parent = NULL;								\
		element->next->parent = element;						\
		first_element = element;								\
    }

#define APPEND_ELEMENT(type_of_element_t, first_element, element) \
	if (first_element==NULL)                            \
    {													\
		first_element = element;                        \
		element->next   = NULL; /* useless */           \
		element->parent = NULL; /* useless */ }         \
	else                                                \
    {													\
		type_of_element_t *f;                           \
		for (f=first_element; f->next!=NULL; f=f->next) \
		{ }												\
		f->next    = element;                           \
		element->parent = f;                            \
		element->next   = NULL;                         \
    }


//--------------------------------------------------------------
/*struct __eXosip_sockaddr {
  u_char ss_len;
  u_char ss_family;
  u_char padding[128 - 2];
};*/

int pub_dns_get_local_fqdn (char **servername, char **serverip,
			char **netmask, unsigned int WIN32_interface) ;


int pub_guess_ip_for_via (int family, char *address, int size) ;


int pub_guess_localip(int family, char *address, int size) ;


char *strdup_printf(const char *fmt, ...) ;

unsigned int via_branch_new_random(void) ;

/*将地址转化为字串*/
char * pub_inet_ntop (struct sockaddr *sockaddr) ;

int pub_get_addrinfo (struct addrinfo **addrinfo, char *hostname, int service) ;

/*将字符串转化为int*/
unsigned int uhash(char * src);

#endif
