/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiQingqi
 */
//0820

#include "uinfo.h"
//extern uinfo_t *user_info_list;
/*
uinfo_t::uinfo_t(char *url,char *login,char *passwd)
{
//	aor_t *aaor;
//	osip_uri_t *dest;
	osip_uri_t *uri;
	int i;
	i=osip_uri_init(&uri);
	if(i==0)
	{
	osip_uri_parse(uri,url);
	status=2;//*not present*
	aor=NULL;
	login=NULL;
	passwd=NULL;
	aor_third_parties=NULL;
	bindings=NULL;
	next=NULL;
	parent=NULL;
	}
	new aor_t(uri);
}
*/
aor_t::aor_t(osip_uri_t *url)
{
	uri=NULL;
	next=NULL;
	parent=NULL;
	osip_uri_t *dest;
	int i=osip_uri_clone(url,&dest);
	if(i==0)
		uri=dest;
}
aor_t::aor_t(aor_t *src)
{
	

	if(src!=NULL)
	{
		osip_uri_t *url;
		int i;
		i=osip_uri_clone(src->uri,&url);
		if(i==0)
			uri=url;

	}
}
aor_t::aor_t(char * url)
{
	osip_uri_t *auri;
	
	osip_uri_init(&auri);
	osip_uri_parse(auri,url);
	uri=auri;



}
aor_t::~aor_t()
{
	osip_uri_free(uri);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
binding_t::binding_t(binding_t *src)
{
	if(src!=NULL)
	{	
		osip_contact_t *dest;
		osip_contact_clone(src->contact,&dest);
		contact=dest;
	}
}
binding_t::binding_t(osip_contact_t *con)
{
	osip_contact_t *dest;
	int i;
	i=osip_contact_clone(con,&dest);
	contact=dest;
}
binding_t::binding_t(char *con)
{
	osip_contact_t *dest;
	osip_contact_init(&dest);

	osip_contact_parse(dest,con);
	contact=dest;
}
binding_t::~binding_t()
{
	osip_contact_free(contact);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

uinfo_t::uinfo_t(char *log_in,char *pass_wd)
{
	status=2;/*not present*/
	aor=NULL;
	login=osip_strdup(log_in);
	passwd=osip_strdup(pass_wd);
	aor_third_parties=NULL;
	bindings=NULL;
	next=NULL;
	parent=NULL;
	
}
uinfo_t::~uinfo_t()
{
	osip_free(login);
	osip_free(passwd);
	/*
	aor->~aor_t();
	bindings->~binding_t();
	*/
	//delete aor;
	//delete bindings;
}
void
uinfo_t::uinfo_set_aor(aor_t *a_aor)
{
	aor_t *dest=new aor_t(a_aor);
	aor=dest;
}
void
uinfo_t::uinfo_set_binding(binding_t *a_bind)
{
	binding_t *dest=new binding_t(a_bind);
	bindings=dest;

}
int 
uinfo_t::add_binding(osip_contact_t *con)
{

	return 0;
}

uinfo_t *
uinfo_t::find_user_info_by_aor(char *aor,uinfo_t *list)
{
	uinfo_t *cur=list;
	while(cur != NULL)
	{
		char *dest;
		//char *dest2;
		int i;
		i=osip_uri_to_str(cur->aor->uri,&dest);
		if(i == 0)
		{
		if(!strcmp(aor,dest))
		{
			//osip_uri_to_str(cur->bindings->contact->url,&dest2);
			osip_free(dest);	
			return cur;
		}
		}
		osip_free(dest);
		cur=cur->next;
	}
	
	return NULL;
}
