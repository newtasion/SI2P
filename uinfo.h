/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiQingQi
 */
//0820

#ifndef _UINFO_H_
#define _UINFO_H_
//#ifdef __cplusplus
//extern "C"
//{
#include <osip2/osip.h>
#include <osipparser2/osip_parser.h>
//#endif
//#ifdef __cplusplus
//}
//#endif
//#include "ChordId.h"
class aor_t
{
public:
	osip_uri_t *uri;
	aor_t *next;
	aor_t *parent;
public:
	aor_t(osip_uri_t *url);
	aor_t(char * url);
	aor_t(aor_t *src);
	~aor_t();

};



class binding_t
{
public:
	osip_contact_t *contact;
	char *path;
	int when;
	binding_t *next;
	binding_t *parent;
public:
	binding_t(osip_contact_t *con);
	binding_t(char *con);
	binding_t(binding_t *src);
	~binding_t();

};



class uinfo_t
{
public:
	int status;
	
//	ChordId uid;
	
	aor_t *aor;
	char *login;
	char *passwd;
	aor_t *aor_third_parties;
	binding_t *bindings;
	
	uinfo_t *parent;
	uinfo_t *next;
public:
	uinfo_t();
	uinfo_t(char *url,char *login,char *passwd);
	uinfo_t(char *login,char *passwd);
	~uinfo_t();


	void uinfo_set_aor(aor_t *aor);
	void uinfo_set_binding(binding_t *bind);

	static uinfo_t *find_user_info_by_aor(char *aor,uinfo_t *list);
	int add_binding(osip_contact_t *con);
	int remove_binding(osip_contact_t *con);
	int user_register();
	int user_unregister();
	

};


#endif
