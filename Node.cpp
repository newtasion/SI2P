/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang , LiQingqi
 */
//0820
#include <string.h>
#include <math.h>
#include <fstream>

#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define snprintf _snprintf
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "baseunit.h"
#include "Node.h"
#include "pubtool.h"
#include "ClosedState.h"
#include "NormalState.h"
#include "uinfo.h"
#include "nodestate.h"
#include "DiscovingState.h"
/* 全局对象 */
Node *mynode ;


//-------------------------------------------------------------------

int Node::initlog(char *filename) 
{
	ofstream fout(filename); 
	fout << "Here is a number: " << "\n";
	fout << flush;
	fout.close() ;
	return 0 ;
}

//-------------------------------------------------------------------

/*初始化将前承后继,finger表都设为自身
单节点状态*/
Node::Node(char * ip , char * port, char * ua_name)
{

	int i ;
	user_info_list=NULL;
	local_user_info_list=NULL;
	red_user_info_list=NULL;

	strcpy(localip,ip) ;

	strcpy(localport,port) ;

	if(user_agent)
	strcpy(user_agent,ua_name) ;

	constants = new Constants(NULL) ; 
	
	chordId = ChordId(localip , localport , constants) ;

	ip_family = AF_INET;

	j_bootstraps = NULL ; 

	expires = 3600 ;

	fingerTable = new FingerTable(constants, this)  ;

	gl_lock = osip_mutex_init ();

	#ifdef WIN32
		//-- Initializing windows socket library
		{
			WORD wVersionRequested;
			WSADATA wsaData;

			wVersionRequested = MAKEWORD(1,1);
			if(int i = WSAStartup(wVersionRequested,  &wsaData))
			{
				OSIP_TRACE (osip_trace
				 (__FILE__, __LINE__, OSIP_ERROR, NULL,
				 "node: Unable to initialize WINSOCK, reason: %d\n",i));
		    }
		}
	#endif

	adosip = new AdOsip() ;
	
	th_transport = new ThreadTransport(this) ;
	user_info_pipe=new jpipe_t();

	i  = initlog(LOGPATH) ;
	if (i < 0 )
	{
		printf("log open failed!") ;
	}

	//set the state 
	_state = new ClosedState() ;
	_state->Handle(this , INIT_SND , NULL , NULL) ;
	
}

//-------------------------------------------------------------------
Node::~Node()
{
	//delete fileout ;
	BootStrap *bs ;
	for (bs = j_bootstraps; bs!=NULL;bs = j_bootstraps)
    {
		REMOVE_ELEMENT(j_bootstraps, bs);
		SAFE_DELETE(bs) ;
    }


	if (_state)
		delete _state ;

	osip_mutex_destroy (gl_lock);

	if(th_stabilize)
		delete th_stabilize ;
	if(th_transport)
		delete th_transport ;
	if(adosip)
		delete adosip ;
	if(fingerTable)
		delete fingerTable ;
	if(user_info_pipe)
		delete user_info_pipe;
	if(constants)
		delete constants ;
}

//-------------------------------------------------------------------
int Node::startthread() 
{
	int i ;
	//start the transport thread 
	i = th_transport->ThreadStart((void *(*)(void *))mythread_transport,this->th_transport) ;
	if (i != 0)
		return -1;
	
	//start
	i = adosip->ThreadStart((void *(*)(void *))mythread_adosip,this->adosip) ;
	if (i != 0)
		return -1;
	return 0 ;	
}

//---------------------------------------------------------------------
void *mythread_transport(void *arg)
{
	ThreadTransport *tran = (ThreadTransport *)arg;
	int i;

	//call the execute func of the ThreadTransport object
	i = tran->Transport_execute(5, 0, -1);	/* infinite loop */
	if (i != 0)
		return NULL;
	return NULL;
	//
}
//---------------------------------------------------------------------
void *mythread_adosip(void *arg) 
{
	AdOsip *adsip = (AdOsip *)arg ;
	int i ;
	
	//call the execute func of the ThreadTransport object
	i = adsip->AdOsip_execute();	/* infinite loop */
	if (i != 0)
		return NULL;
	return NULL;
	//
}

//-------------------------------------------------------------------

void *mythread_stabilize(void *arg) 
{
	Node *mynode = (Node *)arg ;
	while(1)
	{
		osip_usleep(STABILIZE_PERIOD) ;
		mynode->stabilize() ;
	}
}

//-------------------------------------------------------------------

int Node::StartStabilize() 
{
	th_stabilize = new ThreadBase() ;
	int i ;
	//start the transport thread 
	i = th_stabilize->ThreadStart((void *(*)(void *))mythread_stabilize,this) ;
	if (i != 0)
		return -1;
	return 0 ;
}

//-------------------------------------------------------------------

ChordId Node::getChordId() 
{
    return chordId;
}

//-------------------------------------------------------------------

void Node::setChordId(ChordId chordId) 
{
    chordId = chordId;
}

//-------------------------------------------------------------------

FingerTable *Node::getFingerTable() 
{
	return fingerTable;
}

//-------------------------------------------------------------------

Constants *Node::getConstants() 
{
	return constants ;
}

//-------------------------------------------------------------------

void Node::debug() 
{
	return ;
}

//-----------------------------------------------------------------------

ChordId Node::getABootStrap()
{
	//get the first
	if (j_bootstraps)
	{
		return ChordId(
			j_bootstraps->getid(),
			constants,
			(char *)j_bootstraps->getaddress(),
			j_bootstraps->getport() 
			) ;
	}
	else
	{
		ChordId EmptyId ;
		return EmptyId ;
	}
}

//-----------------------------------------------------------------------

NS_EVENT Node::ReportRCVEvent(osip_message_t *M) 
{
	osip_generic_param_t *param ;
	osip_uri_param_get_byname(M->from->gen_params,"user",&param) ;

	//
	if(!strcmp(param->gvalue,"multicast"))
	{
		if(MSG_IS_REQUEST(M))
		{
			return MULTICAST_REQ_RCV ;
		}
		else
		{
			if(MSG_IS_STATUS_2XX(M))
				return MULTICAST_RESP_2XX ;
			else 
				return EVT_OTHER ;
		}
	}

	//
	else if(!strcmp(param->gvalue,"join"))
	{
		if(MSG_IS_REQUEST(M))
		{
			return JOIN_REQ_RCV ;
		}
		else
		{
			if(MSG_IS_STATUS_2XX(M))
				return JOIN_RESP_2XX ;
			else if(MSG_IS_STATUS_3XX(M))
				return JOIN_RESP_3XX ;
			else 
				return EVT_OTHER ;
		}
	}
	
	//
	else if(!strcmp(param->gvalue,"findsucc"))
	{
		if(MSG_IS_REQUEST(M))
		{
			return FINDSUCC_REQ_RCV ;
		}
		else
		{
			if(MSG_IS_STATUS_2XX(M))
				return FINDSUCC_RESP_2XX ;
			else if(MSG_IS_STATUS_3XX(M))
				return FINDSUCC_RESP_3XX ;
			else 
				return EVT_OTHER ;
		}
	}
	
	//
	else if(!strcmp(param->gvalue,"stabilize"))
	{
		if(MSG_IS_REQUEST(M))
		{
			return STABILIZE_REQ_RCV ;
		}
		else
		{
			if(MSG_IS_STATUS_2XX(M))
				return STABILIZE_RESP_2XX ;
			else 
				return EVT_OTHER ;
		}
	}
	
	//
	else if(!strcmp(param->gvalue,"leave"))
	{
		if(MSG_IS_REQUEST(M))
		{
			return LEAVE_REQ_RCV ;
		}
		else 
			return EVT_OTHER ;
	}
	
	//
	else if(!strcmp(param->gvalue,"user_registration"))
	{
		if(MSG_IS_REQUEST(M))
		{
			return USER_REG_RCV ;
		}
		else if(MSG_IS_STATUS_2XX(M))
		
			return USER_REG_RESP_2XX;

		else if(MSG_IS_STATUS_3XX(M))

			return USER_REG_RESP_3XX;
		
		else return EVT_OTHER ;

	}
	else if(!strcmp(param->gvalue,"red_register"))
	{
		if(MSG_IS_REQUEST(M))
			return RED_REGISTER_RCV;
		else if(MSG_IS_STATUS_2XX(M))
			return RED_REGISTER_RESP_2XX;
		else return EVT_OTHER;
	}
	//
		else if(!strcmp(param->gvalue,"user_query"))
	{
		if(MSG_IS_REQUEST(M))
		{
			return USER_QUERY_RCV ;
		}
		else if(MSG_IS_STATUS_2XX(M))
		
			return USER_QUERY_RESP_2XX;

		else if(MSG_IS_STATUS_3XX(M))

			return USER_QUERY_RESP_3XX;
		
		else return EVT_OTHER ;

	}
	//
	else
	{
		return EVT_OTHER ;
	}	
}

//-------------------------------------------------------------------

void Node::setSuccListWithContacts(osip_message_t *M,BOOL includefirst) 
{
	int i ;
	osip_contact_t *osip_contact ;
	if (includefirst)
		i = 0 ;
	else
		i= 1 ;
	int j = 0 ;

	//清空后继列表
	getFingerTable()->clearSuccList() ;

	while(1)
	{
		if(i > NSUCCLIST || osip_message_get_contact(M,i,&osip_contact) < 0)
			break ;	//溢出
		ChordId succi(
			atoi(osip_contact->url->username),
			constants,
			osip_contact->url->host,
			osip_contact->url->port
			) ;
		getFingerTable()->setSuccessor(j,succi) ;
		//后继循环到本身已经没有意义
		if(succi.equals(chordId))
			break ;
		
		j++;
		i++ ;
	}
	return ;
}

//-------------------------------------------------------------------

void Node::setPredWithContacts(osip_message_t *M) 
{
	ChordId pred = getPredWithContact(M) ;	
	getFingerTable()->setPredecessor(pred) ;
//	TransferUserInfoToPred(pred);
}

//-------------------------------------------------------------------

ChordId Node::getPredWithContact(osip_message_t *M) 
{
	osip_contact_t *osip_contact ;
	osip_generic_param_t *param ;
	osip_message_get_contact(M,0,&osip_contact) ;
	int i = osip_contact_param_get_byname (osip_contact, "predecessor", &param);
	osip_uri_t *uri;
	i=osip_uri_init(&uri);
	if (i!=0) { fprintf(stderr, "cannot allocate\n"); return -1; }
	i=osip_uri_parse(uri, param->gvalue);
	if (i!=0) { fprintf(stderr, "cannot parse uri\n"); }
	ChordId new_predessor(
		atoi(uri->username),
		constants,
		uri->host,
		uri->port
		) ;
	osip_uri_free(uri);

	return new_predessor ;
}

//-------------------------------------------------------------------

void Node::set_node_inactive(ChordId node) 
{
	ChordId previous = chordId ;
	printf("update fingertable\n");
	for(int i =fingerTable->getTableLength() ; i >= 1 ; i--)
	{
		if(fingerTable->getFinger(i)->node.equals(node))
			fingerTable->getFinger(i)->node = previous ;
		else
			previous = fingerTable->getFinger(i)->node ;
	}
	printf("update successor list\n");
	ChordId EmptyId ;
	for(i = 0 ; i < getFingerTable()->getSuccNum() ; i++)
	{
		if(getFingerTable()->getSuccessor(i).equals(node))
		{
		//	setSuccessor(i , EmptyId) ;
			ChordId next_succ=getFingerTable()->getSuccessor(i+1);
			
			if (next_succ.equals(EmptyId ))
			{
				getFingerTable()->setSuccessor(i,next_succ);
				break;
			}
			else if (next_succ.equals(getChordId()))
			{
				getFingerTable()->setSuccessor(i,next_succ);
				getFingerTable()->setSuccessor(i+1,EmptyId);
				break;
			}
			else
			{
				getFingerTable()->setSuccessor(i,next_succ);
				getFingerTable()->setSuccessor(i+1,node);
			}
		}
	}

}

//-------------------------------------------------------------------

void Node::OnRegister(osip_message_t *request,osip_transaction_t *tr) 
{
	//1.包括验证ip和id对应 2.出现和本节点id相同，但ip不同 ，3.鉴权
	/*if(!IsValid(request))
		return ;*/

	//---
	NS_EVENT event ;
	event = ReportRCVEvent(request) ;
	_state->Handle(this , event , tr , request) ;

}

//-------------------------------------------------------------------

void Node::OnRegisterSuccess(osip_transaction_t * tr ,osip_message *response) 
{
	//---
	NS_EVENT event ;
	event = ReportRCVEvent(response) ;
	_state->Handle(this , event , tr , response) ;
}

//-------------------------------------------------------------------

void Node::OnRegisterRedirect(osip_transaction_t * tr , osip_message *response) 
{
	//---
	NS_EVENT event ;
	event = ReportRCVEvent(response) ;
	_state->Handle(this , event , tr , response) ;

}

//-------------------------------------------------------------------
void Node::OnRegisterTimeout(osip_transaction_t *tr)
{
	//超时包括在发现阶段超时 和 其它阶段超时
	NS_EVENT event;

	
	
		event=NODE_FAILURE;
		_state->Handle(this,event,tr,NULL);
	

}
void Node::OnDiscoverTimeout(osip_transaction_t *tr)
{
	NS_EVENT event;
	if(_state->getKind() == 2)//DISCOVINGSTATE
	{
		event= NO_NODE_FIND;
		_state->Handle(this,event,tr,NULL);
	}
}
//-------------------------------------------------------------------
/*
void Node::OnRegisterTimeout2(osip_transaction_t *tr)
{
#if 0
	if(_state == ST_NORMAL)
	{
		ChordId node(atoi(tr->orig_request->to->url->username),
					constants,
					tr->orig_request->to->url->host,
					tr->orig_request->to->url->port);
	
		printf("node %d failed \n",node.GetId());
		/*update predecessor*/
/*		
		int succ_fail=0;
		if(node.equals(getSuccessor(0)))
			
		{	

			printf("node %d's successor changed\n",node.GetId());
			//setSuccessor(0,getFingerTable()->getFinger(1)->node);
		 
			if(node.equals(getPredecessor()))// 网络中只剩下一个节点
			{
				printf("node %d's predecessor changed\n",node.GetId());
				//setPredecessor(chordId);
			}
			else succ_fail=1;
		}
		   //Predecessor failed
		else if(node.equals(getPredecessor()))
			{
				ChordId null_id;
				setPredecessor(null_id);

				
			}
		/*update successor list and fingertable*/
/*			set_node_inactive(node);
			ChordId requri,to,contact;
			if(succ_fail == 1)
			{
			requri=getSuccessor(0);
			to=requri;
			contact=getChordId();
			SndRegister(STABILIZE,requri,to,contact,false);
			}

	}
#else
	if(1)
	{
		ChordId node(atoi(tr->orig_request->to->url->username),
					constants,
					tr->orig_request->to->url->host,
					tr->orig_request->to->url->port);
		ChordId localnode=getChordId();
	
		printf("node %d failed \n",node.GetId());
		/*update predecessor*/
		
/*		
		if(node.equals(getFingerTable()->getSuccessor(0)))
			
		{	
			if(node.equals(getFingerTable()->getPredecessor()))
			{
				set_node_inactive(node);
				getFingerTable()->setPredecessor(localnode);
				getFingerTable()->setSuccessor(0,localnode);
			}
			else
			{
				set_node_inactive(node);
				ChordId succ=getFingerTable()->getSuccessor(0);
				SndRegister(LEAVE,succ,node,node,true);
			}

		}
		

		   

	}


#endif //0
}
*/
//-------------------------------------------------------------------

void Node::OnRegisterFailure(osip_transaction_t *tr,osip_message_t *response)
{

}

//-------------------------------------------------------------------

char * Node::ChordId2Uri(ChordId cId,BOOL isSearch) 
{
	char *dest = NULL ;
	
	if(isSearch)	//a search
	{
		dest = (char *) osip_malloc (4 + strlen(cId.GetId_str()) + 8 + 2) ;
		if (dest == NULL)
			return NULL;
		sprintf (dest, "sip:%s@SI2P.COM", cId.GetId_str());
	}
	else
	{
		dest = (char *) osip_malloc (4 + strlen(cId.GetId_str()) + 
			strlen(cId.GetAddress()) + strlen(cId.GetPort()) + 3) ;
		if (dest == NULL)
			return NULL;
		sprintf (dest, "sip:%s@%s:%s", cId.GetId_str(),cId.GetAddress(),cId.GetPort());
	}
	
	return dest ;
}

//-------------------------------------------------------------------


int Node::Set_Fingers(int start_index , ChordId node) 
{
	int i = start_index ;
	getFingerTable()->getFinger(i)->node = node ;
	
	//往后试探
	while(i <= NBITS-1)
	{
		if
		(
			getFingerTable()->getFinger(i+1)->start.BelongsRightInclusive(chordId,getFingerTable()->getFinger(i)->node)
			||getFingerTable()->getFinger(i+1)->start.equals(chordId)
		)
			getFingerTable()->getFinger(i+1)->node = getFingerTable()->getFinger(i)->node ;
		else 
			return i ;

		i++ ;
	}
	return i ;
}

//-------------------------------------------------------------------
/*	找到chordId前面的最近的finger*/
ChordId Node::closestPrecedingFinger(ChordId k) 
{
	//如果失败返回空节点
	ChordId tmp ;
	//至少也必须提供后继
	if (getFingerTable()->getSuccNum() > 0 && !getFingerTable()->getSuccessor(0).equals(chordId))
		tmp = getFingerTable()->getSuccessor(0) ;	

	for (int i = getFingerTable()->getTableLength() ; i > 0; i--) 
	{
        if(!getFingerTable()->getFinger(i)->node.IsEmptyChordId() 
			&& getFingerTable()->getFinger(i)->node.belongs(chordId,k))
		{
			tmp = getFingerTable()->getFinger(i)->node ;
			break ;
		}
	}
/*
	for(i = 0 ; i < getFingerTable()->getSuccNum() ; i++)
	{
		
		if(!getFingerTable()->getSuccessor(i).IsEmptyChordId()
			&& getFingerTable()->getSuccessor(i).belongs(chordId,tmp))
		{
			tmp = getFingerTable()->getSuccessor(i) ;
			break ;
		}
	}
*/
	
	return tmp ;
}

//-----------------------------------------------------------------------

int Node::SndRegister(REG_TYPE type , 
					  ChordId req_uri ,  
					  ChordId to , 
					  ChordId contact , 
					  BOOL IncludeSuccList) 
{
	char *message1;
	unsigned int len = 0;



	osip_transaction_t *transaction;
	osip_event_t *sipevent;
	osip_message_t *request;

	/* temp uri */
	char * tmp_uri;
	
	int i;

	i = osip_message_init(&request);
	if (i!=0) 
		goto si2perror1;
	/* prepare the request-line */
	osip_message_set_method(request, osip_strdup("REGISTER"));
	osip_message_set_version(request, osip_strdup("SIP/2.0"));
	osip_message_set_status_code(request, 0);
	osip_message_set_reason_phrase(request, NULL);

	//requset uri
	if(type == MULTICAST)
	{
		tmp_uri = osip_strdup("sip:224.0.1.75") ;
	}
	else
		tmp_uri = ChordId2Uri(req_uri,false) ;

	osip_uri_t *uri;

	i=osip_uri_init(&uri);
	if (i!=0) 
		goto si2perror2;
	i=osip_uri_parse(uri, tmp_uri );
	if(tmp_uri)	osip_free(tmp_uri) ;
	if (i!=0)
		goto si2perror2;

	osip_message_set_uri(request , uri) ;

	if(type == JOIN)
	{
		tmp_uri = ChordId2Uri(to,false) ;

		/* when JOIN , to and from are same */
		osip_message_set_to(request, tmp_uri );
		osip_message_set_from(request, tmp_uri);
		if (tmp_uri) osip_free(tmp_uri) ;
		osip_from_param_add (request->from, osip_strdup ("user"), osip_strdup("join"));
	}

	else if(type == FINDSUCC)
	{
		tmp_uri = ChordId2Uri(to,true) ;
		osip_message_set_to(request, tmp_uri );
		if (tmp_uri) osip_free(tmp_uri) ;

		tmp_uri = ChordId2Uri(chordId,false);
		osip_message_set_from(request, tmp_uri  );
		if (tmp_uri) osip_free(tmp_uri) ;
		osip_from_param_add (request->from, osip_strdup ("user"), osip_strdup("findsucc"));
		
		//has no contact
	}

	else if(type == STABILIZE)
	{
		tmp_uri = ChordId2Uri(to,false);
		osip_message_set_to(request, tmp_uri);
		if (tmp_uri) osip_free(tmp_uri) ;

		tmp_uri = ChordId2Uri(chordId,false)  ;
		osip_message_set_from(request, tmp_uri );
		osip_from_param_add (request->from, osip_strdup ("user"), osip_strdup("stabilize"));	
		if (tmp_uri) osip_free(tmp_uri) ;
		
		//contact
		tmp_uri = ChordId2Uri(chordId,false);
		char * pre_uri = ChordId2Uri(getFingerTable()->getPredecessor(),false) ;

		char *ctt = (char *) osip_malloc(strlen(tmp_uri) + strlen(";predecessor=") + strlen(pre_uri) +1) ;
		if (ctt == NULL)
			return NULL;
		sprintf (ctt, "%s;predecessor=%s", tmp_uri,pre_uri);
		osip_free(tmp_uri) ;	osip_free(pre_uri) ;

		osip_message_set_contact(request, ctt );
		osip_free(ctt) ;
	}

	else if(type == LEAVE)
	{
		tmp_uri = ChordId2Uri(to,false) ;
		osip_message_set_to(request, tmp_uri );
		if (tmp_uri) osip_free(tmp_uri) ;

		tmp_uri = ChordId2Uri(chordId,false) ;
		osip_message_set_from(request, tmp_uri );
		if (tmp_uri) osip_free(tmp_uri) ;
		osip_from_param_add (request->from, osip_strdup ("user"), osip_strdup("leave"));
		
		//contact
		tmp_uri = ChordId2Uri(chordId,false);
		char * pre_uri = ChordId2Uri(getFingerTable()->getPredecessor(),false) ;

		char *ctt = (char *) osip_malloc(strlen(tmp_uri) + strlen(";predecessor=") + strlen(pre_uri) +1) ;
		if (ctt == NULL)
			return NULL;
		sprintf (ctt, "%s;predecessor=%s", tmp_uri,pre_uri);
		osip_free(tmp_uri) ;	osip_free(pre_uri) ;

		osip_message_set_contact(request, ctt );
		osip_free(ctt) ;
		
		//succlist
		if(IncludeSuccList)
		{
			for(i = 0  ; i < getFingerTable()->getSuccNum() ; i++)
			{
				tmp_uri = ChordId2Uri(getFingerTable()->getSuccessor(i),false) ;
				osip_message_set_contact(request, tmp_uri );
				osip_free(tmp_uri) ;
			}
		}
	}//type == LEAVE
	
	if(type == MULTICAST)
	{
		
		tmp_uri = ChordId2Uri(chordId,false);
		/* when JOIN , to and from are same */
		osip_message_set_to(request, tmp_uri );
		osip_message_set_from(request, tmp_uri);
		osip_from_param_add (request->from, osip_strdup ("user"), osip_strdup("multicast"));
		osip_free(tmp_uri) ;
		//no contact
	}//type == MULTIPL


	//---set call_id and cseq
	osip_call_id_t *callid;
	osip_cseq_t *cseq;
	char *num;
	char  *cidrand;
	char *register_callid_number ;

	/* call-id is always the same for REGISTRATIONS */
	i = osip_call_id_init(&callid);
	if (i!=0) 
		goto si2perror2;
	cidrand = osip_strdup("BF9598C48B184EBBAFADFE527EED8186") ;
	osip_call_id_set_number(callid, cidrand);
	register_callid_number = cidrand;

	osip_call_id_set_host(callid, osip_strdup("SI2P.COM"));
	request->call_id = callid;

	//cseq
	i = osip_cseq_init(&cseq);
	if (i!=0) 
		goto si2perror2 ;
	num = osip_strdup("1");
	osip_cseq_set_number(cseq, num);
	osip_cseq_set_method(cseq, osip_strdup("REGISTER"));
	request->cseq = cseq;
	 
	/*the Max-Forward header */
	osip_message_set_max_forwards(request, "5"); /* a UA should start a request with 70 */

	/* the via header */
	char tmp[200];
	unsigned int branch;
	branch=osip_build_random_number();
    snprintf(tmp, 200, "SIP/2.0/%s %s:%s;rport;branch=z9hG4bK%u", "UDP",
	      localip,
	      localport,
	      branch );

    osip_message_set_via(request, tmp);

	/* the UA header */
	osip_message_set_user_agent(request, user_agent);

	/*  the expires header */
	char exp[10]; /* MUST never be ouside 1 and 3600 */
	snprintf(exp, 9, "%i", expires);
	osip_message_set_expires(request, exp);
	osip_message_set_content_length(request, "0");


	/*** then must wake up osip ***/
	i = osip_transaction_init(&transaction,
		       NICT,
		       adosip->j_osip,
		       request);
	if (i!=0)
		goto si2perror2 ;

	//jr->r_last_tr = transaction;

	/* send REGISTER */
	
	i = osip_message_to_str(request, &message1, &len);
	LogStream("SEND======================================>>\n") ;
	LogStream(message1) ;
	if(message1)	osip_free(message1) ;
//	printf("SEND======================================>>\n") ;
//	printf(message1) ;

	sipevent = osip_new_outgoing_sipmessage(request);
	sipevent->transactionid =  transaction->transactionid;
	osip_message_force_update(request);
  
	osip_transaction_add_event(transaction, sipevent);

	adosip->ThreadWakeUp();
	return 0;

si2perror1:
	if(request != NULL)osip_message_free(request);
	return -1 ;
si2perror2:
	if(request != NULL)osip_message_free(request);
	return -1;
}

//-------------------------------------------------------------------

static char *
osip_to_tag_new_random ()
{
  char *tmp = (char *) osip_malloc (33);
  unsigned int number = osip_build_random_number ();

  sprintf (tmp, "%u", number);
  return tmp;
}

int Node::SndResponse(int status , 
					  osip_message_t *request , 
					  osip_transaction_t *tr,
					  REG_TYPE type , 
					  ChordId contact, 
					  BOOL IncludeSuccList)  
{
	char *message1;
	size_t length = 0;
	
	//
	osip_generic_param_t *tag;
	osip_message_t *response;
	osip_event_t *evt ;
	char *tmp;
	char * tmp_uri;
	int pos;
	int i;

	i = osip_message_init (&response);
	if (i != 0)
		return -1;

	osip_message_set_version (response, osip_strdup ("SIP/2.0"));
	osip_message_set_status_code (response, status);

	tmp = osip_strdup(osip_message_get_reason (status));
	if (tmp == NULL)
		osip_message_set_reason_phrase (response, osip_strdup ("Unknown status code"));
	else
		osip_message_set_reason_phrase (response, tmp);

	osip_message_set_method (response, NULL);
	osip_message_set_uri (response, NULL);

	i = osip_to_clone (request->to, &(response->to));
	if (i != 0)
		goto si2perror1;

	i = osip_to_get_tag (response->to, &tag);
	if (i != 0)
    {	/* we only add a tag if it does not already contains one! */
		if (status == 200 && MSG_IS_REGISTER (request))
		{
			osip_to_set_tag (response->to, osip_to_tag_new_random ());
		}
		else if (status >= 200)
		{
			osip_to_set_tag (response->to, osip_to_tag_new_random ());
		}
    }

	i = osip_from_clone (request->from, &(response->from));
	if (i != 0)
		goto si2perror1;
	
	pos = 0;
	while (!osip_list_eol (request->vias, pos))
    {
		osip_via_t *via;
		osip_via_t *via2;

		via = (osip_via_t *) osip_list_get (request->vias, pos);
		i = osip_via_clone (via, &via2);
		if (i != -0)
			goto si2perror1;
		osip_list_add (response->vias, via2, -1);
		pos++;
    }

	i = osip_call_id_clone (request->call_id, &(response->call_id));
	if (i != 0)
		goto si2perror1;
	i = osip_cseq_clone (request->cseq, &(response->cseq));
	if (i != 0)
		goto si2perror1;

	//set server
	osip_message_set_server (response, osip_strdup ("SI2P"));

	/*add contact*/
	//predecessor
	if(status == 200)
	{
		if(type == USER_REGISTRATION || type == TRANSFER_REGISTRATION || type == THIRD_PARTY)
		{
			osip_contact_t *tmp;
			char *dest;
			int pos=0;
			while(pos<osip_list_size(request->contacts))
			{
				pos=osip_message_get_contact(request,0,&tmp);
				osip_contact_to_str(tmp,&dest);
				osip_message_set_contact(response,dest);
				if(dest)	osip_free(dest) ;
				pos++;
			}
		}
	
		else
		{
			tmp_uri = ChordId2Uri(chordId,false);
			char * pre_uri = ChordId2Uri(getFingerTable()->getPredecessor(),false) ;

			char *ctt = (char *) osip_malloc(strlen(tmp_uri) + strlen(";predecessor=") + strlen(pre_uri) +1) ;
			if (ctt == NULL)
				return NULL;
			sprintf (ctt, "%s;predecessor=%s", tmp_uri,pre_uri);
			osip_free(tmp_uri) ;	osip_free(pre_uri) ;

			osip_message_set_contact(response, ctt );
			osip_free(ctt) ;
		}
	}
	else//302
	{
		if(type == USER_REGISTRATION || type == TRANSFER_REGISTRATION || type == THIRD_PARTY)
		{
			tmp_uri = ChordId2Uri(contact,false);
			osip_message_set_contact(response,tmp_uri );
			osip_free(tmp_uri) ;
		}
		else
		{
			tmp_uri = ChordId2Uri(contact,false) ;
			char *ctt = (char *) osip_malloc(strlen(tmp_uri) + strlen(";predecessor=notprovided") +1) ;
			if (ctt == NULL)
				return NULL;
			sprintf (ctt, "%s;predecessor=notprovided", tmp_uri);
			if(tmp_uri) osip_free(tmp_uri) ;	
			osip_message_set_contact(response, ctt );
			if(tmp_uri) osip_free(ctt) ;
		}
		
	}

	if(IncludeSuccList)
	{
		for(i = 0  ; i < getFingerTable()->getSuccNum() ; i++)
		{
			tmp_uri = ChordId2Uri(getFingerTable()->getSuccessor(i),false) ;
			osip_message_set_contact(response, tmp_uri );
			osip_free(tmp_uri) ;
		}
	}

	
	i = osip_message_to_str(response, &message1, &length);
	LogStream("SEND======================================>>\n") ;
	LogStream(message1) ;
	if(message1)	osip_free(message1) ;
//	printf("SEND======================================>>\n") ;
//	printf(message1) ;

	evt = osip_new_outgoing_sipmessage (response);
  	evt->transactionid = tr->transactionid;
	osip_transaction_add_event(tr, evt);

  	adosip->ThreadWakeUp();

	return 0;

si2perror1:
	osip_message_free (response);
	return -1;
}

//-------------------------------------------------------------------

void Node::StartNode(BOOL assingle) 
{
	if(assingle)
	{
		//单节点启动时，前承后继和finger表都是本身
		getFingerTable()->setPredecessor(chordId) ;
		getFingerTable()->setSuccessor(0,chordId) ;
		Set_Fingers(1,chordId)  ;
		//
		//-直接进入normal状态
		ChangeState(new NormalState(this)) ;

	}
	else
	{
		Discover() ;

	}
}
void Node::StartNode()
{
	//寻找网络中是否有已启动的节点
	
	ChordId chordId = getChordId() ;
	SndRegister(MULTICAST,chordId,chordId,chordId) ; 
	
	
	NodeState *ns=new DiscovingState();
	ChangeState(ns);

}
//-----------------------------------------------------------------------
int Node::Data_toString(char **buf , unsigned int &length) 
{
	char *buffer = new char[5000] ;
	if (buffer==NULL)
	  return -1;

	char *message = buffer ;

	sprintf(message,"**********NODE STATE**********") ;
	message = message + strlen (message);
	strncpy (message, "\r\n\0", 2);
	message = message + 2;	
	
	sprintf (message, "LOCAL_NODE:	id:%d		ip:%s", 
			chordId.GetId() ,
			chordId.GetAddress() 
			);
	message = message + strlen (message);
	strncpy (message, "\r\n\0", 2);
	message = message + 2;

	strncpy(message,"\0",1) ;
	*buf = buffer ;
	length = strlen(buffer) ;
	return 0 ;			
}

//-------------------------------------------------------------------
void Node::DatatoFile() 
{
	char *temp ;
	unsigned int len ;

	//输出fingertable
	getFingerTable()->toString(&temp , len) ;

	ofstream fout(LOGPATH);

	fout << temp << "\n";
	SAFE_DELETE_ARRAY(temp) ;
	
	char *t ;
	_state->toString(&t , len) ;
	fout << t << "\n";
	SAFE_DELETE_ARRAY(t) ;

	return ;
}
//-------------------------------------------------------------------
void Node::DatatoMonitor() 
{
	char *temp , *t ;
	unsigned int len ;

	ChordId localnode=getChordId();
	unsigned int id=localnode.GetId();
	char *add=localnode.GetAddress();
	printf("Local node,id= %d,address = %s\n",id,add);

	//输出fingertable
	getFingerTable()->toString(&temp , len) ;
	printf(temp) ;
	printf("\n") ;
	SAFE_DELETE_ARRAY(temp) ;
	
	_state->toString(&t , len) ;
	printf(t) ;
	printf("\n") ;
	SAFE_DELETE_ARRAY(t) ;		
	return ;
}

//-------------------------------------------------------------------

void Node::LogStream(const char *out) 
{
	/*
	ofstream fout(LOGPATH,ios::app);
	fout << out << "\n";
	fout << flush;
	fout.close() ;
	*/
}

//-------------------------------------------------------------------
void Node::ChangeState(NodeState *newState)
{
	NodeState * oldState = _state;
	_state = newState;
	delete oldState;
}


//---------------------------------------------------------------------

void Node::Init()
{
	_state->Handle(this , INIT_SND , NULL , NULL) ;
}

//---------------------------------------------------------------------

void Node::Discover()
{
	_state->Handle(this , DISCOVER_SND , NULL , NULL) ;
}

//---------------------------------------------------------------------

void Node::Join()
{
	//打印出bootstrap
	_state->Handle(this , JOIN_SND , NULL , NULL) ;
}

//---------------------------------------------------------------------

void Node::Leave()
{
	_state->Handle(this , LEAVE_SND , NULL , NULL) ;
}

//---------------------------------------------------------------------

void Node::stabilize() 
{
	_state->Handle(this , STABILIZE_SND , NULL , NULL) ;
}

//---------------------------------------------------------------------

void Node::RegisterUser(REG_TYPE type,char * aor,char *contact,char *username,char * passwd)
{
	/*
      REGISTER sip:15@192.168.1.201 SIP/2.0
	  Via:SIP/2.0/UDP 192.168.1.61:15060;branch=z9hG4bkxxxxx
      To:liqq <sip:liqq@SI2P.com>
	  From:liqq <sip:liqq@SI2P.com>
	  Call-ID:
	  CSeq:
	  Contact:<sip:liqq@192.168.1.61>
	  Content-Length:0
	  */
	/*判断本节点是否对该用户信息负责，
	如果负责，添加到user_info_list,
	如果不负责，计算next-hop,并发送REGISTER请求*/
	unsigned int uid;
	uid=uhash(aor);
	ChordId localnode=getChordId();
	ChordId pred=getFingerTable()->getPredecessor();
	Constants constants(NULL);
	ChordId UserID(uid,&constants);

	aor_t aaor(aor);
	binding_t abind(contact);
	uinfo_t *user_info=new uinfo_t(username,passwd);
	user_info->uinfo_set_aor(&aaor);
	user_info->uinfo_set_binding(&abind);
	ADD_ELEMENT(local_user_info_list,user_info);

//	int lid=localnode.GetId();
//	int pid=predecessor.GetId();
	
	//if((lid<pid && uid<=lid && uid>pid)||(lid>pid && (uid < lid || uid >=pid)))
	if(UserID.BelongsRightInclusive(pred,localnode))
	{
		printf("Add a new local user\n");
		uinfo_t *uinfo=new uinfo_t(username,passwd);
		uinfo->uinfo_set_aor(&aaor);
		uinfo->uinfo_set_binding(&abind);
		ADD_ELEMENT(user_info_list,uinfo);
		
		if(pred.equals(localnode))
		{
			uinfo_t *ruinfo=new uinfo_t(username,passwd);
			ruinfo->uinfo_set_aor(&aaor);
			ruinfo->uinfo_set_binding(&abind);
			ADD_ELEMENT(red_user_info_list,ruinfo);
		}
		ChordId succ=getFingerTable()->getSuccessor(0);
		if(!succ.equals(getChordId()))
		{
		string next_hop="sip:";
		next_hop+=succ.GetAddress();
		SndUserRegisterRequest(RED_REGISTER,uinfo,next_hop.c_str() ,3600);
		}
	}
	else
	{
		ChordId	node=closestPrecedingFinger(UserID);
		//ChordId node=getFingerTable()->getPredecessor();
		//ChordId node=getChordId();
		string registrar="sip:";
		registrar+=node.GetAddress();
		SndUserRegisterRequest(USER_REGISTRATION,user_info,registrar.c_str() ,3600);

	}

}
int
Node::SndUserRegisterRequest(REG_TYPE type,
				uinfo_t *user_info,
				const char *registrar,
				int expires)
{
	char *message1;
	unsigned int len = 0;

	osip_message_t *request;
	osip_event_t *sipevent;
	osip_transaction_t *transaction;
	osip_uri_t *uri;
	
	char *tmp_uri;
	int i;

	i=osip_message_init(&request);
	if(i!=0)
		goto si2perror1;
	osip_message_set_method(request,strdup("REGISTER"));
	osip_message_set_version(request,strdup("SIP/2.0"));
	osip_message_set_status_code(request,0);
	osip_message_set_reason_phrase(request,NULL);

	i=osip_uri_init(&uri);
	if(i!=0)
		goto si2perror2;
	i=osip_uri_parse(uri,osip_strdup(registrar));
	osip_message_set_uri(request,uri);

	//*set to,from and header
	if(type==USER_REGISTRATION)
	{	
		char *dest1;
		char *dest2;
		
		osip_uri_to_str(user_info->aor->uri,&dest1);
		i=osip_message_set_to(request,dest1);
		i=osip_message_set_from(request,dest1);
		if(dest1)	osip_free(dest1) ;
		osip_from_param_add (request->from, osip_strdup ("user"), osip_strdup("user_registration"));

		osip_contact_to_str(user_info->bindings->contact,&dest2);
		i=osip_message_set_contact(request,osip_strdup(dest2));
		if(dest2)	osip_free(dest2) ;
	}

	if(type == RED_REGISTER)
	{
		char *dest1;
		char *dest2;
		osip_uri_to_str(user_info->aor->uri,&dest1);
		i=osip_message_set_to(request,dest1);
		i=osip_message_set_from(request,dest1);
		if(dest1)	osip_free(dest1) ;
		osip_from_param_add (request->from, osip_strdup ("user"), osip_strdup("red_register"));

		osip_contact_to_str(user_info->bindings->contact,&dest2);
		i=osip_message_set_contact(request,osip_strdup(dest2));
		if(dest2)	osip_free(dest2) ;
	}

	if(type==TRANSFER_REGISTRATION)
	{
		char *dest1;
		char *dest2;
		//string tmp_uri;
		osip_uri_to_str(user_info->aor->uri,&dest1);
		osip_message_set_to(request,dest1);
		if(dest1)	osip_free(dest1) ;

		tmp_uri = ChordId2Uri(chordId,false);
		osip_message_set_from(request,tmp_uri);
		osip_from_param_add (request->from, osip_strdup ("user"), osip_strdup("transfer_registration"));
		if(tmp_uri)	osip_free(tmp_uri) ;

		osip_contact_to_str(user_info->bindings->contact,&dest2);
		osip_message_set_contact(request,dest2);
		if(dest2)	osip_free(dest2) ;
	}
	if(type==THIRD_PARTY)
	{
		//todo
	}
		//---set call_id and cseq
	osip_call_id_t *callid;
	osip_cseq_t *cseq;
	char *num;
	char  *cidrand;
	char *register_callid_number ;

	//* call-id is always the same for REGISTRATIONS 
	i = osip_call_id_init(&callid);
	if (i!=0) 
		goto si2perror2;
	cidrand = osip_strdup("BF9598C48B184EBBAFADFE527EED8186") ;
	osip_call_id_set_number(callid, cidrand);
	register_callid_number = cidrand;

	osip_call_id_set_host(callid, osip_strdup("SI2P.COM"));
	request->call_id = callid;

	//cseq
	i = osip_cseq_init(&cseq);
	if (i!=0) 
		goto si2perror2 ;
	num = osip_strdup("1");
	osip_cseq_set_number(cseq, num);
	osip_cseq_set_method(cseq, osip_strdup("REGISTER"));
	request->cseq = cseq;
	 
	//*the Max-Forward header 
	osip_message_set_max_forwards(request, "5"); //* a UA should start a request with 70 

	//* the via header 
	char tmp[200];
    snprintf(tmp, 200, "SIP/2.0/%s %s:%s;rport;branch=z9hG4bK%u", "UDP",
	      localip,
	      localport,
	      via_branch_new_random() );

    osip_message_set_via(request, tmp);

	//* the UA header 
	osip_message_set_user_agent(request, user_agent);

	//*  the expires header 
	char exp[10]; //* MUST never be ouside 1 and 3600 
	snprintf(exp, 9, "%i", expires);
	osip_message_set_expires(request, exp);
	osip_message_set_content_length(request, "0");


	//*** then must wake up osip 
	i = osip_transaction_init(&transaction,
		       NICT,
		       adosip->j_osip,
		       request);
	if (i!=0)
		goto si2perror3 ;

	//* send REGISTER 
	i = osip_message_to_str(request, &message1, &len);
	LogStream("SEND======================================>>\n") ;
	LogStream(message1) ;
	if(message1) osip_free(message1) ;
//	printf("SEND======================================>>\n") ;
//	printf(message1) ;
	sipevent = osip_new_outgoing_sipmessage(request);
	sipevent->transactionid =  transaction->transactionid;
	osip_message_force_update(request);
  
	osip_transaction_add_event(transaction, sipevent);

	adosip->ThreadWakeUp();
	return 0;

si2perror1:
	
	return -1;

si2perror2:
	if(request!=NULL)
		osip_message_free(request);
	return -1;

si2perror3:
	if(transaction!=NULL)
		osip_message_free(request);
	return -1;


}
int Node::SndUserRegisterResponse(
					  int status , 
					  osip_message_t *request , 
					  osip_transaction_t *tr,
					  REG_TYPE type , 
					  char *contact)
{
	return 0;
}
int Node::SndUserRegisterRequest(char *aor ,const char *requri)
{
	char *message1;
	unsigned int len = 0;



	osip_transaction_t *transaction;
	osip_event_t *sipevent;
	osip_message_t *request;


	int i;

	i = osip_message_init(&request);
	if (i!=0) 
		goto si2perror1;
	/* prepare the request-line */
	osip_message_set_method(request, osip_strdup("REGISTER"));
	osip_message_set_version(request, osip_strdup("SIP/2.0"));
	osip_message_set_status_code(request, 0);
	osip_message_set_reason_phrase(request, NULL);

	//requset uri


	osip_uri_t *uri;

	i=osip_uri_init(&uri);
	if (i!=0) 
		goto si2perror2;
	i=osip_uri_parse(uri, requri);
	if (i!=0)
		goto si2perror2;

	osip_message_set_uri(request , uri) ;


	//to from ,no contact
	osip_message_set_to(request,aor);
	osip_message_set_from(request,aor);

	osip_from_param_add(request->from,osip_strdup("user"),osip_strdup("user_query"));

	//---set call_id and cseq
	osip_call_id_t *callid;
	osip_cseq_t *cseq;
	char *num;
	char  *cidrand;
	char *register_callid_number ;

	/* call-id is always the same for REGISTRATIONS */
	i = osip_call_id_init(&callid);
	if (i!=0) 
		goto si2perror2;
	cidrand = osip_strdup("BF9598C48B184EBBAFADFE527EED8186") ;
	osip_call_id_set_number(callid, cidrand);
	register_callid_number = cidrand;

	osip_call_id_set_host(callid, osip_strdup("SI2P.COM"));
	request->call_id = callid;

	//cseq
	i = osip_cseq_init(&cseq);
	if (i!=0) 
		goto si2perror2 ;
	num = osip_strdup("1");
	osip_cseq_set_number(cseq, num);
	osip_cseq_set_method(cseq, osip_strdup("REGISTER"));
	request->cseq = cseq;
	 
	/*the Max-Forward header */
	osip_message_set_max_forwards(request, "5"); /* a UA should start a request with 70 */

	/* the via header */
	char tmp[200];
	unsigned int branch;
	branch=osip_build_random_number();
    snprintf(tmp, 200, "SIP/2.0/%s %s:%s;rport;branch=z9hG4bK%u", "UDP",
	      localip,
	      localport,
	      branch );

    osip_message_set_via(request, tmp);

	/* the UA header */
	osip_message_set_user_agent(request, user_agent);

	/*  the expires header */
	char exp[10]; /* MUST never be ouside 1 and 3600 */
	snprintf(exp, 9, "%i", expires);
	osip_message_set_expires(request, exp);
	osip_message_set_content_length(request, "0");


	/*** then must wake up osip ***/
	i = osip_transaction_init(&transaction,
		       NICT,
		       adosip->j_osip,
		       request);
	if (i!=0)
		goto si2perror2 ;

	/* send REGISTER */
	i = osip_message_to_str(request, &message1, &len);
	LogStream("SEND======================================>>\n") ;
	LogStream(message1) ;
	if(message1) osip_free(message1) ;

	sipevent = osip_new_outgoing_sipmessage(request);
	sipevent->transactionid =  transaction->transactionid;
	osip_message_force_update(request);
  
	osip_transaction_add_event(transaction, sipevent);

	adosip->ThreadWakeUp();
	return 0;

si2perror1:
	if(request != NULL)osip_message_free(request);
	return -1 ;
si2perror2:
	if(request != NULL)osip_message_free(request);
	return -1;
	return 0;
}
//---------------------------------------------------------------------
void Node::QueryUser()
{
	/*
	REGISTER sip:31@192.168.1.201
	To: <sip:liqq@SI2P.com>
	From: <sip:liqq@SI2P.com>

*/
	//char *aor="sip:lizang@si2p.com";

	char buf[50];
	printf("input the user aor you want to call\n");
	gets(buf);
	char *aor=buf;
	
	unsigned int uid=uhash(aor);
	Constants constants(NULL);
	ChordId UserID(uid,&constants);
	ChordId pred=mynode->getFingerTable()->getPredecessor();
	ChordId localnode=getChordId();
	
	//for test
	if(UserID.BelongsRightInclusive(pred,localnode))
	{
		printf("\nThe user infomation is on this node\n");
		uinfo_t *user_info=uinfo_t::find_user_info_by_aor(aor,user_info_list);
		char *dest;
		osip_uri_to_str(user_info->bindings->contact->url,&dest);
		printf("user contact is %s\n",dest);
		osip_free(dest);
		return;

	}
	else
	{
		ChordId next_hop=closestPrecedingFinger(UserID);
		//ChordId next_hop=getChordId();
		ChordId empty(-1,&constants);
		if(!next_hop.equals(empty))
		{
			char *tmp=next_hop.GetAddress();
			char *requri = (char *)osip_malloc (4 + strlen(tmp) + 1) ;
			sprintf (requri, "sip:%s", tmp);
			SndUserRegisterRequest(aor,requri);
			if(requri)	osip_free(requri) ;
		}
		else  //do not know where to send
		{
			return ;
		}
		
	}

}

//---------------------------------------------------------------------
int
Node::SndResponse4Query(int status,
						const char *contact,
						osip_transaction_t *tr,
						osip_message_t *request)
{
	char *message1;
	size_t length = 0;
	
	//
	osip_generic_param_t *tag;
	osip_message_t *response;
	osip_event_t *evt ;
	char *tmp;
	int pos;
	int i;

	i = osip_message_init (&response);
	if (i != 0)
		return -1;

	osip_message_set_version (response, osip_strdup ("SIP/2.0"));
	osip_message_set_status_code (response, status);

	tmp = osip_strdup(osip_message_get_reason (status));
	if (tmp == NULL)
		osip_message_set_reason_phrase (response, osip_strdup ("Unknown status code"));
	else
		osip_message_set_reason_phrase (response, tmp);

	osip_message_set_method (response, NULL);
	osip_message_set_uri (response, NULL);

	i = osip_to_clone (request->to, &(response->to));
	if (i != 0)
		goto si2perror1;

	i = osip_to_get_tag (response->to, &tag);
	if (i != 0)
    {	/* we only add a tag if it does not already contains one! */
		if (status == 200 && MSG_IS_REGISTER (request))
		{
			osip_to_set_tag (response->to, osip_to_tag_new_random ());
		}
		else if (status >= 200)
		{
			osip_to_set_tag (response->to, osip_to_tag_new_random ());
		}
    }

	i = osip_from_clone (request->from, &(response->from));
	if (i != 0)
		goto si2perror1;
	
	pos = 0;
	while (!osip_list_eol (request->vias, pos))
    {
		osip_via_t *via;
		osip_via_t *via2;

		via = (osip_via_t *) osip_list_get (request->vias, pos);
		i = osip_via_clone (via, &via2);
		if (i != -0)
			goto si2perror1;
		osip_list_add (response->vias, via2, -1);
		pos++;
    }

	i = osip_call_id_clone (request->call_id, &(response->call_id));
	if (i != 0)
		goto si2perror1;
	i = osip_cseq_clone (request->cseq, &(response->cseq));
	if (i != 0)
		goto si2perror1;

	//set server
	osip_message_set_server (response, osip_strdup ("SI2P"));

	/*add contact*/
	if(contact !=NULL)	
	osip_message_set_contact(response,contact);

	i = osip_message_to_str(response, &message1, &length);
	LogStream("SEND======================================>>\n") ;
	LogStream(message1) ;
	if(message1)	osip_free(message1) ;

	evt = osip_new_outgoing_sipmessage (response);
  	evt->transactionid = tr->transactionid;
	osip_transaction_add_event(tr, evt);

  	adosip->ThreadWakeUp();

	return 0;

si2perror1:
	osip_message_free (response);
	return -1;
}

//---------------------------------------------------------------------
void Node::GetUserInfoFromInput(char **aor,char **contact)
{
	char *buf1=(char *)osip_malloc(50);
	char *buf2=(char *)osip_malloc(50);
	char ip[50];
	int j=0;
	printf("input your aor(max_length:50)\n");
	gets(buf1);
	*aor=buf1;

	pub_guess_localip(AF_INET,ip,49);
	int i=0;
	for(i=0;buf1[i]!='@';i++)
	{
		buf2[i]=buf1[i];

	}
	buf2[i]='@';
	i++;
	for(j=0;j<=strlen(ip);j++)
	{
		buf2[i]=ip[j];
		i++;
	}
	*contact=buf2;
	printf("contact is %s",buf2);
}

//---------------------------------------------------------------------
void
Node::TransferUserInfoToPred(ChordId node)
{
	uinfo_t *red_cur=red_user_info_list;
	uinfo_t *cur=user_info_list;
	while(red_cur != NULL)
	{
		char *red_aor;
		uinfo_t *red_tmp=red_cur;
		red_cur=red_cur->next;
		osip_uri_to_str(red_tmp->aor->uri,&red_aor);
		if(red_aor)	osip_free(red_aor) ;
			
		char * red_registrar= (char *)osip_malloc(4 + strlen(node.GetAddress()) +1) ;
		sprintf (red_registrar, "sip:%s", node.GetAddress());
		SndUserRegisterRequest(RED_REGISTER,red_tmp,red_registrar ,3600);
		if(red_registrar)	osip_free(red_registrar) ;
		
		REMOVE_ELEMENT(red_user_info_list,red_tmp);
		delete red_tmp;
		

	}
	
	ChordId localnode=getChordId();
	unsigned int uid=0;
	while(cur != NULL)
	{
		
		char *aor;
		uinfo_t *tmp=cur;
		cur=cur->next;
		osip_uri_to_str(tmp->aor->uri,&aor);
		uid=uhash(aor);
		if(aor)	osip_free(aor) ;

		Constants constants(NULL);
		ChordId UserID(uid,&constants);
		if(!UserID.BelongsRightInclusive(node,localnode))
		{
			char * registrar= (char *)osip_malloc(4 + strlen(node.GetAddress()) +1) ;
			sprintf (registrar, "sip:%s", node.GetAddress());
			SndUserRegisterRequest(USER_REGISTRATION,tmp,registrar,3600);
			if(registrar)	osip_free(registrar) ;
		
			REMOVE_ELEMENT(user_info_list,tmp);
			//ADD_ELEMENT(red_user_info_list,tmp);//move this user info to redundant user info list
			delete tmp;
		}
	}

}
void 
Node::TransferUserInfoToSucc(ChordId node)
{
	uinfo_t *cur=user_info_list;
	uinfo_t *red_cur=red_user_info_list;

	char * succ= (char *)osip_malloc(4 + strlen(node.GetAddress()) +1) ;
	sprintf (succ, "sip:%s", node.GetAddress());
	const char *registrar=succ ;

	//转移该节点负责的用户信息
	while(cur != NULL)
	{
		uinfo_t *tmp=cur;
		cur=tmp->next;
		SndUserRegisterRequest(USER_REGISTRATION,tmp,registrar,3600);
		REMOVE_ELEMENT(user_info_list,tmp);
		delete tmp;
	}
	
	//转移该节点负责的冗余用户信息
	while(red_cur != NULL)
	{
		uinfo_t *tmp2=red_cur;
		red_cur=tmp2->next;
		SndUserRegisterRequest(RED_REGISTER,tmp2,registrar,3600);
		REMOVE_ELEMENT(red_user_info_list,tmp2);
		delete tmp2;
	}

	if(succ)	osip_free(succ) ;
}


void 
Node::PrintAllUser()
{
	uinfo_t *cur=user_info_list;
	printf("Registered user:\n");
	while(cur != NULL)
	{
		char *aor;
		int id=0;
		osip_uri_to_str(cur->aor->uri,&aor);
		id=uhash(aor);
		printf("UserId=%d, AOR= %s\n",id,aor);
		if(aor)	osip_free(aor) ;
		cur=cur->next;
	}
	cur = local_user_info_list;
	printf("local user:\n");
	while(cur != NULL)
	{
		char *aor;
		int id=0;
		osip_uri_to_str(cur->aor->uri,&aor);
		id=uhash(aor);
		printf("UserId=%d, AOR= %s\n",id,aor);
		if(aor)		osip_free(aor) ;
		cur=cur->next;
	}
	cur = red_user_info_list;
	printf("Redundancy registered user:\n");
	while(cur != NULL)
	{
		char *aor;
		int id=0;
		osip_uri_to_str(cur->aor->uri,&aor);
		id=uhash(aor);
		printf("UserId=%d, AOR= %s\n",id,aor);
		if(aor)	osip_free(aor) ;
		cur=cur->next;
	}
}


void Node::clearUserInfoList(uinfo_t **list)
{
	uinfo_t *cur=*list;
	while(cur != NULL)
	{
		uinfo_t *tmp=cur;
		cur=cur->next;
		REMOVE_ELEMENT((*list),tmp);
		delete tmp;
	}
	*list=NULL;
}
int Node::GetNodeState()
{
	if (_state->_kind == 0)//CLOSESTATE)
		return 0;

	else
	if (_state->_kind == 1)//INITEDSTATE)
		return 1;

	else 
	if (_state->_kind == 2)//DISCOVERINGSTATE)
		return 2;
	
	else 
	if (_state->_kind == 3)//DISCOVEREDSTATE)
		return 3;
	
	else
	if (_state->_kind == 4)//JOININGSTATE)
		return 4;
	
	else
	if (_state->_kind == 5)//FTCOMPUTINGSTATE)
		return 5;

	else 
	if (_state->_kind == 6)//NORMALSTATE)
		return 6;

	else 
		return -1;


}