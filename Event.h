#ifndef __EVENT_H__
#define __EVENT_H__

#include "regapi.h"
/**
 * @defgroup eXosip_event eXosip event Management
 * @ingroup eXosip
 * @{
 */
enum event_type_t {
  /* Registration Info */
  EXOSIP_REGISTRATION_NEW,              /* announce new registration.       */
  EXOSIP_REGISTRATION_SUCCESS,          /* user is successfully registred.  */
  EXOSIP_REGISTRATION_FAILURE,          /* user is not registred.           */
  EXOSIP_REGISTRATION_REFRESHED,        /* registration has been refreshed. */
  EXOSIP_REGISTRATION_TERMINATED,       /* UA is not registred any more.    */

  EXOSIP_CALLBACK_COUNT
} ;

class event_t
{
public:	
	event_type_t		etype;
	void				*external_reference;
	char                reason_phrase[50];
	int                 status_code;
	char                textinfo[256];
	int					rid;
	reg_t				*jr;
public:
	int event_add_status(osip_message_t *response);
	event_t(event_type_t type, reg_t *jr) ;
	event_t(event_type_t type) ;
	~event_t() ;
	
} ;


#endif

