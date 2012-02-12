/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include "NormalState.h"
#include "ClosedState.h"
#include "pubtool.h"
#include "ChordId.h"
#include <string.h>
//----------------------------------------------------------------------

NormalState::NormalState(Node *node)
{
	_kind = NodeState::NORMALSTATE ;
	printf("SWITCH TO NORMALSTATE !\n") ;
	node->StartStabilize() ;
}

//----------------------------------------------------------------------

NormalState::~NormalState()
{
	printf("OUT OF NORMALSTATE !\n") ;
}


//----------------------------------------------------------------------
void NormalState::Leave(Node *node)
{
	ChordId chordId = node->getChordId() ;

	//- Send Register to tell pred&&succ that i will leave
	
	
	ChordId succ=node->getFingerTable()->getSuccessor(0);
	ChordId pred=node->getFingerTable()->getPredecessor();
	if(!succ.IsEmptyChordId() && !succ.equals(chordId))
	{
		int i = node->SndRegister(LEAVE,succ,chordId,chordId,true) ; 
		node->TransferUserInfoToSucc(succ);
	}

	if(!pred.IsEmptyChordId() && !pred.equals(chordId) && !pred.equals(succ) )
	{
		int i = node->SndRegister(LEAVE,pred,chordId,chordId,true) ; 
	}

	
	
	
	//transfer user infomation to successor
	//ChordId successor=node->getFingerTable()->getSuccessor();
	
	//node->TranserUserInfo();
	
	//- change state
	ChangeState(node,new ClosedState()) ;
}

//----------------------------------------------------------------------

void NormalState::onDiscoverReg( 
					Node* node,
					osip_transaction_t *tr,
					osip_message_t *request )
{
	//打印数据流日志
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(request, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
	printf("RECEIVE<<======================================\n") ;
	printf(message) ;

	ChordId to(
			atoi(request->to->url->username),
			node->getConstants(),
			request->to->url->host,
			request->to->url->port
			) ;
	ChordId from(
			atoi(request->from->url->username),
			node->getConstants(),
			request->from->url->host,
			request->from->url->port
			) ;
	
	ChordId chordId = node->getChordId() ;
	//***********************************

	node->SndResponse(200 , request , tr , JOIN , chordId, false) ;	

}

//----------------------------------------------------------------------

void NormalState::onJoinReg( 
				Node* node,
				osip_transaction_t *tr,
				osip_message_t *request )
{
	//打印数据流日志
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(request, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;

	ChordId to(
			atoi(request->to->url->username),
			node->getConstants(),
			request->to->url->host,
			request->to->url->port
			) ;
	ChordId from(
			atoi(request->from->url->username),
			node->getConstants(),
			request->from->url->host,
			request->from->url->port
			) ;

	ChordId tmp_node ;

	ChordId chordId = node->getChordId() ;
	//***********************************


	/* if to is not equals Nid and Nsucc , then look up finger table */
	if(to.BelongsRightInclusive(node->getFingerTable()->getPredecessor(),chordId))
	{
		if(chordId.equals(node->getFingerTable()->getSuccessor(0)))	//如果和后继相等
		{
			node->SndResponse(200 , request , tr , JOIN ,chordId,false) ;
			//node->TransferUserInfoToPred(to);
		}
		else
		{
			node->SndResponse(200 , request , tr , JOIN ,chordId,true) ;	
			//node->TransferUserInfoToPred(to);
		}
	}
	else
	{
		tmp_node = node->closestPrecedingFinger(to) ;
		if (tmp_node.GetId() != -1)
			node->SndResponse(302 , request , tr , JOIN , tmp_node,false) ;	
		else
			node->SndResponse(404 , request , tr , JOIN , tmp_node,false) ;	
	}

	return ;
} 

//----------------------------------------------------------------------

void NormalState::onFindSuccReg( 
				Node* node,
				osip_transaction_t *tr,
				osip_message_t *request )
{
	//打印数据流日志
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(request, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;

	ChordId to(
			atoi(request->to->url->username),
			node->getConstants(),
			request->to->url->host,
			request->to->url->port
			) ;
	ChordId from(
			atoi(request->from->url->username),
			node->getConstants(),
			request->from->url->host,
			request->from->url->port
			) ;

	ChordId tmp_node ;
	ChordId chordId = node->getChordId() ;
	//***********************************


	if (to.BelongsRightInclusive(node->getFingerTable()->getPredecessor(),chordId))	//归本节点负责
	{
		node->SndResponse(200 , request , tr , FINDSUCC , chordId,false) ;	
		
	}
	else	//不是本节点负责，必须重定向
	{
		tmp_node  = node->closestPrecedingFinger(to) ;
		if (tmp_node.GetId() != -1)
			node->SndResponse(302, request , tr , FINDSUCC , tmp_node , false) ;
		else
			node->SndResponse(404 , request , tr , FINDSUCC , tmp_node,false) ;
	}

	return ;
} 

//----------------------------------------------------------------------

void NormalState::onStabilizeReg( 
				Node* node,
				osip_transaction_t *tr,
				osip_message_t *request )
{
	//打印数据流日志
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(request, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;

	ChordId to(
			atoi(request->to->url->username),
			node->getConstants(),
			request->to->url->host,
			request->to->url->port
			) ;
	ChordId from(
			atoi(request->from->url->username),
			node->getConstants(),
			request->from->url->host,
			request->from->url->port
			) ;

	ChordId tmp_node ;
	ChordId chordId = node->getChordId() ;

	//***********************************


	if (from.belongs(node->getFingerTable()->getPredecessor(), chordId))
	{
		node->getFingerTable()->setPredecessor(from) ;
		node->TransferUserInfoToPred(from);
	}
	if (from.belongs(chordId , node->getFingerTable()->getSuccessor(0)))
	{
		node->getFingerTable()->setSuccessor(0, from) ;
		node->Set_Fingers(1,from) ;	
	}

	//this happens when a node failure was dectected
	//if (!(from.belongs(node->getFingerTable()->getPredecessor(), chordId))&&
	//	!(from.belongs(chordId , node->getFingerTable()->getSuccessor(0))))
	//	node->getFingerTable()->setPredecessor(from);

	
	if(chordId.equals(node->getFingerTable()->getSuccessor(0)))	//如果和后继相等
	{
		node->SndResponse(200 , request , tr , JOIN ,chordId,false) ;	
	}
	else
		node->SndResponse(200 , request , tr , STABILIZE , chordId,true);
	
	return ;
} 

//----------------------------------------------------------------------

void NormalState::onLeaveReg( 
				Node* node,
				osip_transaction_t *tr,
				osip_message_t *request )
{
	//打印数据流日志
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(request, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;

	ChordId to(
			atoi(request->to->url->username),
			node->getConstants(),
			request->to->url->host,
			request->to->url->port
			) ;
	ChordId from(
			atoi(request->from->url->username),
			node->getConstants(),
			request->from->url->host,
			request->from->url->port
			) ;

	ChordId tmp_node ;
	ChordId pred=node->getFingerTable()->getPredecessor();
	ChordId succ=node->getFingerTable()->getSuccessor(0);
	if(to.equals(from)) //正常离开
	{
		if (to.equals(pred))
		{
			node->setPredWithContacts(request);
			printf("clean redundant user info list\n");
			node->clearUserInfoList(&(node->red_user_info_list));
		}
		if (to.equals(succ))
		{
			node->setSuccListWithContacts(request,false) ;
			node->set_node_inactive(to) ;
		}
	}
	else //非正常离开
	{
		if(to.equals(pred))
		{
			node->getFingerTable()->setPredecessor(from);
			uinfo_t *cur=node->red_user_info_list;
			ChordId succ=node->getFingerTable()->getSuccessor(0);
			//string registrar="sip:";
			//registrar+=succ.GetAddress();
			char *reg_address=osip_strdup(succ.GetAddress());
			char *registrar=(char *)osip_malloc(strlen("sip:")+strlen(reg_address)+1);
			sprintf(registrar,"sip:%s",reg_address);
			osip_free(reg_address);
			while (cur != NULL)
			{
				uinfo_t *tmp=cur;
				cur =cur->next;
				REMOVE_ELEMENT(node->red_user_info_list,tmp);
				ADD_ELEMENT(node->user_info_list,tmp);
				
				if(!succ.equals(node->getChordId()))
				node->SndUserRegisterRequest(RED_REGISTER,tmp,registrar,3600);
				osip_free(registrar);
			}
			
			Stabilize(node);
			
		}
		if (to.equals(succ))
		{
			node->set_node_inactive(to);
			Stabilize(node);
		}
	}
	
}


//----------------------------------------------------------------------

void NormalState::onFindSuccOK(
					Node* node,
					osip_transaction_t * tr ,
					osip_message *response) 
{
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(response, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;

	ChordId to(
			atoi(response->to->url->username),
			node->getConstants(),
			response->to->url->host,
			response->to->url->port
			) ;

	osip_contact_t * osip_contact ;
	osip_message_get_contact(response,0,&osip_contact) ;
	if(!osip_contact)	
	{return ;}//error
	ChordId contact(
		atoi(osip_contact->url->username),
		node->getConstants(),
		osip_contact->url->host,
		osip_contact->url->port
		) ;

	ChordId chordId = node->getChordId() ;

	//******************************


	//scan fingertable
	for(i = 1 ; i <= node->getFingerTable()->getTableLength() ; i++)
	{
		if(node->getFingerTable()->getFinger(i)->start.equals(to)) 
			break ;
	}
	
	int k ;
	if(i <= node->getFingerTable()->getTableLength())
		k = node->Set_Fingers(i, contact) + 1 ;

	//A.finger表更新完毕，有两种情况
	if (k == node->getFingerTable()->getTableLength() + 1)
	{
		return ;
	}

	//B.finger表未更新完
	if (k<= node->getFingerTable()->getTableLength())	//update of fingertalbe if not completed
	{
		int i = node->SndRegister(FINDSUCC,contact,node->getFingerTable()->getFinger(k)->start,chordId) ;

		return ;
	}
}

//----------------------------------------------------------------------

void NormalState::onStabilizeOK(
					Node* node,
					osip_transaction_t * tr ,
					osip_message *response) 
{
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(response, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;

	ChordId to(
			atoi(response->to->url->username),
			node->getConstants(),
			response->to->url->host,
			response->to->url->port
			) ;

	osip_contact_t * osip_contact ;
	osip_message_get_contact(response,0,&osip_contact) ;
	if(!osip_contact)	
	{return ;}//error
	ChordId contact(
		atoi(osip_contact->url->username),
		node->getConstants(),
		osip_contact->url->host,
		osip_contact->url->port
		) ;

	ChordId chordId = node->getChordId() ;

	//******************************

	int update_finger_from = 1 ;

	if (node->getFingerTable()->getSuccessor(0).equals(to)) //if the message is from succ
	{
		//succ's predecessor
		ChordId tmp_pred = node->getPredWithContact(response) ;

		//..后继有变动
		if (tmp_pred.belongs(chordId , node->getFingerTable()->getSuccessor(0)))
		{
			//要重新更新finger表
			update_finger_from = node->Set_Fingers(1,tmp_pred) + 1;	// update some fingers
			
			//以前的succ全部失效
			node->getFingerTable()->clearSuccList() ;
			node->getFingerTable()->setSuccessor(0,tmp_pred) ;	//update immediate succ
		}

		//..后继正确
		else if (tmp_pred.equals(chordId))
		{
			update_finger_from = node->Set_Fingers(1,contact) + 1;	// update some fingers
			node->setSuccListWithContacts(response,true) ;
		}

		//..不处理
		else
		{
			return ;
		}
	}

	else if (node->getFingerTable()->getPredecessor().equals(to))	//from predcessor
	{
		osip_contact_t *osip_contact ;
		osip_message_get_contact(response,1,&osip_contact) ;
		ChordId tmp_succ(
			atoi(osip_contact->url->username),
			node->getConstants(),
			osip_contact->url->host,
			osip_contact->url->port
			) ;
		
		//..前承有变动
		if (tmp_succ.belongs(node->getFingerTable()->getPredecessor() , chordId))
		{
			node->getFingerTable()->setPredecessor(tmp_succ) ;
			node->TransferUserInfoToPred(tmp_succ);
		}
		//..前承正确
		else
		{
			return ;
		}
	}
	else
	{
		return ;
	}

	/*when updating of succ/pred is completed , it is time to update fingertable*/
	
	if (update_finger_from <= node->getFingerTable()->getTableLength())	//fingertable需要更新其他表项
	{
		//ChordId req_uri = closestPrecedingFinger(fingerTable->getFinger(update_finger_from)->start) ;
		ChordId req_uri = node->getFingerTable()->getSuccessor(0) ;

		if(!req_uri.equals(chordId) && !req_uri.IsEmptyChordId())
		{
			int i = node->SndRegister(
				FINDSUCC,
				req_uri,
				node->getFingerTable()->getFinger(update_finger_from)->start,
				chordId) ; 
		}
		else
		{
			return ;
		}
	}
	else//fingertable不需要更新其他表项
	{
		//stabilizing = 0 ;
	}

}

//----------------------------------------------------------------------

void NormalState::onFindSuccRedirect(
					Node* node,
					osip_transaction_t * tr ,
					osip_message *response) 
{
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(response, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;

	/* to , from , contact are same as orig_request ;
	req_uri is the contact of the response */
	ChordId to(
			atoi(tr->orig_request->to->url->username),
			node->getConstants(),
			tr->orig_request->to->url->host,
			tr->orig_request->to->url->port
			) ;
	ChordId from(
			atoi(tr->orig_request->from->url->username),
			node->getConstants(),
			tr->orig_request->from->url->host,
			tr->orig_request->from->url->port
			) ;

		
	osip_contact_t *osip_contact ;
	osip_message_get_contact(response,0,&osip_contact) ;
	if(!osip_contact)	
	{return ;}//error
	ChordId contact(
		atoi(osip_contact->url->username),
		node->getConstants(),
		osip_contact->url->host,
		osip_contact->url->port
		) ;


	ChordId chordId = node->getChordId() ;
	//******************************

	//如果重定向到自己,那么自身就是对应to的finger
	if(contact.equals(chordId))
	{
		//scan fingertable
		for(int i = 1 ; i <= node->getFingerTable()->getTableLength() ; i++)
		{
			if(node->getFingerTable()->getFinger(i)->start.equals(to)) 
				break ;
		}

		int k ;
		if(i <= node->getFingerTable()->getTableLength())
			k = node->Set_Fingers(i, contact) + 1 ;

		//A.finger表更新完毕
		if (k == node->getFingerTable()->getTableLength() + 1)
		{

		}

		//B.finger表未更新完
		if (k<= node->getFingerTable()->getTableLength())	//update of fingertalbe if not completed
		{
			int i = node->SndRegister(FINDSUCC,contact,node->getFingerTable()->getFinger(k)->start,chordId) ; 
		}	
	}

	//继续重定向
	else
	{
		node->SndRegister(FINDSUCC,contact,to,chordId) ; 		
	}

}

//----------------------------------------------------------------------

void NormalState::Stabilize(Node * node) 
{
	ChordId chordId = node->getChordId() ;

	if(!node->getFingerTable()->getSuccessor(0).equals(chordId))
	{
		//mynode->stabilizing = 1 ;
		int i = node->SndRegister(
			STABILIZE,
			node->getFingerTable()->getSuccessor(0),
			node->getFingerTable()->getSuccessor(0),
			chordId) ; 
	}

	else if(!node->getFingerTable()->getPredecessor().IsEmptyChordId() 
		&& !node->getFingerTable()->getSuccessor(0).equals(chordId))//only one node ???
	{
		node->Set_Fingers(1,node->getFingerTable()->getPredecessor()) ;
	}//	else if
	else
	{}

	if(!node->getFingerTable()->getPredecessor().equals(node->getFingerTable()->getSuccessor(0)) 
		&& !node->getFingerTable()->getPredecessor().IsEmptyChordId()
		&& !node->getFingerTable()->getPredecessor().equals(chordId))
	{
		//mynode->stabilizing = 1 ;
		int i = node->SndRegister(STABILIZE,
			node->getFingerTable()->getPredecessor(),
			node->getFingerTable()->getPredecessor(),
			chordId) ; 
	}
}

//----------------------------------------------------------------------
void NormalState::onUserReg(Node *node,
							osip_transaction_t *tr,
							osip_message_t *request)
{
	bool isquery=false;
	char *message;
	size_t length = 0;
	int j = osip_message_to_str(request, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;


	/*计算aor的hash值，判断是否对该用户信息负责*/
	
	osip_to_t *to;
	char *aor;
	
	int i,uid;
	
	to=osip_message_get_to(request);
	i=osip_uri_to_str(to->url,&aor);
	uid=uhash(aor);
	Constants constants(NULL);
	ChordId UserID(uid,&constants);
	ChordId localnode=node->getChordId();
	ChordId pred=node->getFingerTable()->getPredecessor();
	//本地负责
	if(UserID.BelongsRightInclusive(pred,localnode))//for test 
	{
		
		aor_t aaor(aor);
		
		osip_contact_t *acontact;
		char *contact;
		osip_message_get_contact(request,0,&acontact);
		osip_contact_to_str(acontact,&contact);
		binding_t abind(contact);

		uinfo_t *uinfo=new uinfo_t(NULL,NULL);
		uinfo->uinfo_set_aor(&aaor);
		uinfo->uinfo_set_binding(&abind);
		printf("Add a new remote user\n");
		ADD_ELEMENT(node->user_info_list,uinfo);
		node->SndResponse(200,request,tr,USER_REGISTRATION,UserID,false);

		//冗余注册
		ChordId succ=node->getFingerTable()->getSuccessor(0);
		if(!succ.equals(node->getChordId()))
		{
			
			char *next_addr=osip_strdup(succ.GetAddress());
			char * next_hop=(char *)osip_malloc(strlen("sip:")+strlen(next_addr)+1);
			sprintf(next_hop,"sip:%s",next_addr);
			osip_free(next_addr);
			node->SndUserRegisterRequest(RED_REGISTER,uinfo,next_hop,3600);
			osip_free(next_hop);
		}
	
	
	}
	//本地不负责
	else
	{
		
		ChordId next_hop=node->closestPrecedingFinger(UserID);
		ChordId empty_id(-1);
		if(next_hop.equals(empty_id))
		node->SndResponse(404,request,tr,USER_REGISTRATION,empty_id,false);
		else
		//	next_hop
		//ChordId next_hop=node->getChordId();
		/*string tmp="sip:";
		tmp+=next_hop.GetAddress();
		osip_contact_init(&next_address);
		osip_contact_parse(next_address,tmp.c_str());
		*/
		node->SndResponse(302,request,tr,USER_REGISTRATION,next_hop,false);
	}
	
}

//----------------------------------------------------------------------
void NormalState::onUserRegOK(Node *node,
									osip_transaction_t *tr,
									osip_message_t *response
									)

{
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(response, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;
	printf("\nuser register success!\n");


}
//----------------------------------------------------------------------
void NormalState::onUserRegRedirect(Node *node,
									osip_transaction_t *tr,
									osip_message_t *response
									)

{
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(response, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;

	uinfo_t *user_info;
	osip_contact_t *contact;
//	string registrar="sip:";
	 
		
	osip_message_get_contact(response,0,&contact);
	char *host=osip_strdup(contact->url->host);
	char *registrar=(char *)osip_malloc(strlen("sip:")+strlen(host)+1);
	sprintf(registrar,"sip:%s",host);
	osip_free(host);
	
	char *aor =NULL;
	osip_uri_to_str(osip_message_get_to(response)->url,&aor);
	
	if(aor != NULL)
	{
		user_info=uinfo_t::find_user_info_by_aor(aor,node->local_user_info_list);
		osip_free(aor);
		node->SndUserRegisterRequest(USER_REGISTRATION,user_info,registrar,3600);
	}
	osip_free(registrar);
}
void NormalState::onUserQuery(Node *node,
							osip_transaction_t *tr,
							osip_message_t *request)
{
	bool isquery=false;
	char *message;
	size_t length = 0;
	int j = osip_message_to_str(request, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;



	/*计算aor的hash值，判断是否对该用户信息负责*/
	
	osip_to_t *to;
	char *aor;
	
	int i,uid;
	
	to=osip_message_get_to(request);
	i=osip_uri_to_str(to->url,&aor);
	uid=uhash(aor);
	Constants constants(NULL);
	ChordId UserID(uid,&constants);
	ChordId localnode=node->getChordId();
	ChordId pred=node->getFingerTable()->getPredecessor();
	//本地负责
	if(UserID.BelongsRightInclusive(pred,localnode))//for test 
	{
			uinfo_t * user_info=uinfo_t::find_user_info_by_aor(aor,node->user_info_list);
			if (user_info != NULL)
			{
				//todo
				char *contact;
				osip_uri_to_str(user_info->bindings->contact->url,&contact);
				node->SndResponse4Query(200,contact,tr,request);
			}
	
	}
	//本地不负责
	else
	{
		
		ChordId next_hop=node->closestPrecedingFinger(UserID);
		ChordId empty_id(-1);
		if(next_hop.equals(empty_id))
		//node->SndResponse(404,request,tr,USER_REGISTRATION,empty_id,false);
		node->SndResponse4Query(404,NULL,tr,request);
		//	next_hop
		//ChordId next_hop=node->getChordId();
		/*string tmp="sip:";
		tmp+=next_hop.GetAddress();
		osip_contact_init(&next_address);
		osip_contact_parse(next_address,tmp.c_str());
		*/
		else
		{

/*		string tmp;
		tmp=node->ChordId2Uri(next_hop,false);
		node->SndResponse4Query(302,tmp.c_str(),tr,request);
*/
			char *tmp=node->ChordId2Uri(next_hop,false);
			node->SndResponse4Query(302,tmp,tr,request);
		}//node->SndResponse(302,request,tr,USER_REGISTRATION,next_hop,false);
	}
	
}

//----------------------------------------------------------------------
void NormalState::onUserQueryOK(Node *node,
									osip_transaction_t *tr,
									osip_message_t *response
									)

{
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(response, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;
	printf("\nuser query success!\n");
	osip_contact_t *contact;
	osip_message_get_contact(response,0,&contact);
    memset(node->user_contact,0,200);
	
	char *dest;
	osip_contact_to_str(contact,&dest);
	//printf("user contact %s",dest);

	for(int j=0;j<strlen(dest);j++)
	{
		node->user_contact[j]=dest[j];
	}
	node->user_contact[j]=0;
	node->user_info_pipe->jpipe_write("w",1);
	
	osip_free(dest);
	
	//we can set up a session now


}
//----------------------------------------------------------------------
void NormalState::onUserQueryRedirect(Node *node,
									osip_transaction_t *tr,
									osip_message_t *response
									)

{
	char *message;
	size_t length = 0;
	int i = osip_message_to_str(response, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;

//	uinfo_t *user_info;
	osip_contact_t *contact;
//	string registrar="sip:";

	char *host;
	char *registrar;
	osip_message_get_contact(response,0,&contact);
	host=osip_strdup(contact->url->host);
	registrar=(char *)osip_malloc(strlen("sip:")+strlen(host)+1);
	sprintf(registrar,"sip:%s",host);
	osip_free(host);
	
	
	char *aor =NULL;
	osip_uri_to_str(osip_message_get_to(response)->url,&aor);
	
	if(aor != NULL)
	{
	//	user_info=uinfo_t::find_user_info_by_aor(aor,local_user_info_list);
		
		//node->SndUserRegisterRequest(USER_REGISTRATION,user_info,registrar.c_str(),3600);
		node->SndUserRegisterRequest(aor,registrar);
		osip_free(aor);
	}
	osip_free(registrar);

}

//----------------------------------------------------------------------
void NormalState::onNodeFailure(Node *node,osip_transaction_t *tr)
{
	osip_message_t *request=tr->orig_request;

	Constants constants(NULL);
	ChordId to(atoi(request->to->url->username),
		&constants,
		request->to->url->host,
		request->to->url->port
		);
	ChordId succ=node->getFingerTable()->getSuccessor(0);
	ChordId pred=node->getFingerTable()->getPredecessor();
	ChordId localnode=node->getChordId();
	if(to.equals(succ))
	{
		printf("\nNode %d failed \n",to.GetId());
		

		if(to.equals(pred))
		{
			node->getFingerTable()->setPredecessor(localnode);
			node->set_node_inactive(to);
			uinfo_t *cur=node->red_user_info_list;
			while(cur != NULL)
			{
				uinfo_t *tmp=cur;
				cur=cur->next;
				REMOVE_ELEMENT(node->red_user_info_list,tmp);
				ADD_ELEMENT(node->user_info_list,tmp);
			}
		}
		else
		{
			succ=node->getFingerTable()->getSuccessor(1);
			node->set_node_inactive(to);
			
			node->SndRegister(LEAVE,succ,to,NULL,true);
			uinfo_t *cur=node->user_info_list;
			

			char *registrar;
			char *reg_address=osip_strdup(succ.GetAddress());
			registrar=(char *)osip_malloc(strlen("sip:")+strlen(reg_address)+1);
			sprintf(registrar,"sip:%s",reg_address);
			osip_free(reg_address);

			while(cur != NULL)
			{
				uinfo_t *tmp=cur;
				cur=cur->next;
				node->SndUserRegisterRequest(RED_REGISTER,tmp, registrar,3600);

			}
			osip_free(registrar);
		
		}
		
	}
}
//----------------------------------------------------------------------
void NormalState::onRedRegister(Node *node,osip_transaction_t *tr,osip_message_t *request)
{

	char *message;
	size_t length = 0;
	int j = osip_message_to_str(request, &message, &length);
	node->LogStream("RECEIVE<<======================================\n") ;
	node->LogStream(message) ;
//	printf("RECEIVE<<======================================\n") ;
//	printf(message) ;

	
	osip_to_t *to;
	char *aor;
	ChordId empty(-1);
	

	
		to=osip_message_get_to(request);
		osip_uri_to_str(to->url,&aor);
		aor_t aaor(aor);
		
		osip_contact_t *acontact;
		char *contact;
		osip_message_get_contact(request,0,&acontact);
		osip_contact_to_str(acontact,&contact);
		binding_t abind(contact);

		uinfo_t *uinfo=new uinfo_t(NULL,NULL);
		uinfo->uinfo_set_aor(&aaor);
		uinfo->uinfo_set_binding(&abind);
		printf("Add a new redundant user\n");
		ADD_ELEMENT(node->red_user_info_list,uinfo);
		node->SndResponse(200,request,tr,USER_REGISTRATION,empty,false);

}

//----------------------------------------------------------------------
void NormalState::onRedRegisterOK(Node *node,osip_transaction_t *tr,osip_message_t *message)
{
	printf("redundancy register ok\n");
}
//----------------------------------------------------------------------
void NormalState::Handle(Node* node ,
					NS_EVENT event ,
					osip_transaction_t * tr ,
					osip_message *message) 
{
	//
	switch(event)
	{
	case INIT_SND :
		break ;

	case DISCOVER_SND :
		break ;

	case JOIN_SND :
		Join(node) ;
		break ;

	case LEAVE_SND :
		Leave(node) ;
		break ;

	case STABILIZE_SND :
		Stabilize(node) ;
		break ;

	case MULTICAST_REQ_RCV:
		
		onDiscoverReg( 
			node,
			tr ,
			message) ;
		break ;
	
	case MULTICAST_RESP_2XX:
		//- DiscovingState消息响应，驱动进入DiscoveredState状态
		onDiscoverOK(
			node,
			tr ,
			message) ;

		break ;
	
	case JOIN_REQ_RCV:

		onJoinReg( 
			node,
			tr ,
			message) ;
		break ;

	case JOIN_RESP_2XX:
		//- JoiningState消息响应，驱动进入FTComputingState/NormalState状态
		onJoinOK(
			node,
			tr ,
			message) ;

		break ;

	case JOIN_RESP_3XX:
		//- JoiningState消息响应，继续JoiningState状态
		onJoinRedirect(
			node,
			tr ,
			message) ;

		break ;

	case FINDSUCC_REQ_RCV:

		onFindSuccReg( 
			node,
			tr ,
			message) ;
		break ;

	case FINDSUCC_RESP_2XX:

		onFindSuccOK( 
			node,
			tr ,
			message) ;
		break ;

	case FINDSUCC_RESP_3XX:

		onFindSuccRedirect( 
			node,
			tr ,
			message) ;
		break ;

	case STABILIZE_REQ_RCV:

		onStabilizeReg( 
			node,
			tr ,
			message) ;
		break ;

	case STABILIZE_RESP_2XX:

		onStabilizeOK( 
			node,
			tr ,
			message) ;
		break ;

	case LEAVE_REQ_RCV:

		onLeaveReg( 
			node,
			tr ,
			message) ;
		break ;

	case USER_REG_RCV:
		
		onUserReg(
			node,
			tr,
			message);
		break;

	case USER_REG_RESP_2XX:
		onUserRegOK(
			node,
			tr,
			message);
		break;

	case USER_REG_RESP_3XX:
		onUserRegRedirect(
			node,
			tr,
			message);
		break;
	
	case USER_QUERY_RCV:
		//TODO :we need call a function
		onUserQuery(
			node,
			tr,
			message);
		break;

	case USER_QUERY_RESP_2XX:
		onUserQueryOK(
			node,
			tr,
			message);
		break;

	case USER_QUERY_RESP_3XX:
		onUserQueryRedirect(
			node,
			tr,
			message);
		break;

	case NODE_FAILURE:
		
		onNodeFailure(
			node,
			tr);

		break;
	
	case RED_REGISTER_RCV:

		onRedRegister(
			node,
			tr,
			message);
		break;

	case RED_REGISTER_RESP_2XX:
		onRedRegisterOK(
			node,
			tr,
			message);
		break;




	
	
	case EVT_OTHER:
		break ;

	
	}
}
