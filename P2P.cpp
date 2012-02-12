/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#include "Node.h"
#include "pubtool.h"

#include "p2p.h"



void cmdloop()
{
	char buf[256];

	while(1)
	{
		char *p;
		int ret = -1;
		
		#ifndef WIN32
			printf("please input your command:") ;
			fgets(buf, sizeof(buf), stdin);
		#else // !WIN32
			gets(buf);
		#endif // !WIN32
			p = strstr(buf, "\n");
		if (p)
			*p = 0;

		switch(buf[0])
		{
		case 'c':
		case 'C':
			delete mynode ;
			return;
		case 'p':
		case 'P':
			mynode->DatatoMonitor() ;
			mynode->PrintAllUser();
			break;
		case 'f':
		case 'F':
			mynode->DatatoFile() ;
			break;
		case 'L':
		case 'l':
			si2p_node_leave() ;
			break;
		case 'J':
		case 'j':
			si2p_node_join() ;
			break ;
		case 'B':
		case 'b':
			si2p_node_start2() ;
			break ;
		case 'd':
		case 'D':
			si2p_node_start(true) ;
			break ;
		case 'h':
		case 'H':
			break;
		case 'u':
		case 'U':
			char *aor;
			aor="sip:b@b";
			
			si2p_register_user(aor);
			//printf("now register user\n");
			//mynode->GetUserInfoFromInput;(&aor,&contact);
			//aor="sip:liqq@si2p.com";
			//contact="sip:liqq@192.168.1.61";
			//mynode->RegisterUser
				//(USER_REGISTRATION,aor,contact,NULL,NULL);
			//osip_free(aor);
			//osip_free(contact);
			break;
	

		case 'q':
		case 'Q':
			mynode->QueryUser();
			char *contact;
			int i=1;
			i=si2p_query_user("sip:lizang@sip.com",&contact);
			if(i == 0)
				printf("user contact :%s\n",contact);
			else printf("can not find the user\n");
			break;

		}
    }
}
/*
void main()
{
	si2p_p2p_init();
	cmdloop();
}

*/

//---------------------------------------------------------------
int si2p_register_user(char *aor)
{
	char localip[50];
	char contact[200];
	int i=0;
	int j=0;

	pub_guess_localip(AF_INET,localip,49);
	for(i=0;i<=strlen(aor);i++)
	{
		if(aor[i]=='@')
		{
			contact[i]='@';
			break;
		}
		else contact[i]=aor[i];

	}
	i++;
	//int test=aor[16];
	for (j=0;j<strlen(localip);j++)
	{
		contact[i]=localip[j];
		i++;
	}
	contact[i]=0;

	mynode->RegisterUser(USER_REGISTRATION,aor,contact,NULL,NULL);
	return 0;

}

//---------------------------------------------------------------
int si2p_query_user(char *aor,char **contact)	
{
	
	int uid=uhash(aor);
	Constants constants(NULL);
	ChordId UserID(uid,&constants);
	ChordId localnode=mynode->getChordId();
	ChordId pred=mynode->getFingerTable()->getPredecessor();

	if(UserID.BelongsRightInclusive(pred,localnode))//本地负责
	{
		uinfo_t *uinfo=uinfo_t::find_user_info_by_aor(aor,mynode->user_info_list);
		if(uinfo != NULL) 
		{
			osip_uri_to_str(uinfo->bindings->contact->url,contact);
			return 0;
		}
		else 
			return -1;
	}
	
	else  //其他节点负责
	{
		ChordId next_hop=mynode->closestPrecedingFinger(UserID);
		ChordId empty(-1,&constants);
		if(!next_hop.equals(empty))
		{
			string next_addr="sip:";
			next_addr+=next_hop.GetAddress();
			mynode->SndUserRegisterRequest(aor,next_addr.c_str());
		}
		
		char *buf=(char *)osip_malloc(200);
		fd_set fd;
		int s=mynode->user_info_pipe->jpipe_get_read_descr();
		struct timeval tv;
		tv.tv_sec=10;
		tv.tv_usec=0;
		FD_ZERO(&fd);
		FD_SET(s,&fd);

		//int i=select(s+1,&fd,NULL,NULL,NULL);
		int i=select(s+1,&fd,NULL,NULL,&tv);
		if(FD_ISSET(s,&fd))
		{
			int len=strlen(mynode->user_contact);
			for(i=0;i<len;i++)
			{
				buf[i]=mynode->user_contact[i];

			}
			buf[i]=0;
			*contact=buf;
			return 0;

		}
		else 
			return -1;

		
	}

}
int si2p_p2p_init()
{
	int i = 0 ;
	char locip[50];
	int ip_family = AF_INET ;

	//得到本地IP的方法
	pub_guess_ip_for_via(ip_family, locip, 49);
	if (locip[0]=='\0')
    {
		OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
         "eXosip: No ethernet interface found!\n"));
		OSIP_TRACE (osip_trace
        (__FILE__, __LINE__, OSIP_ERROR, NULL,
         "eXosip: using 127.0.0.1 (debug mode)!\n"));
		strcpy(locip, "127.0.0.1");
		/* we should always fallback on something. The linphone user will surely
		start linphone BEFORE setting its dial up connection.*/
    }
	
	mynode = new Node(locip,"15062","SI2P") ;

	if(i<0)
	{
		printf("\nSI2P: Can't Start Node!") ;
		exit(1) ;
	}
	else
	{
		printf("\nnode has initialized!\n") ;
	}
	return 0;
	
}
void  si2p_node_leave()
{
	mynode->Leave() ;
	
}
void si2p_node_join()
{
	mynode->Join() ;
}


void si2p_node_start(bool asingle)
{
	mynode->StartNode(asingle) ;
}

void si2p_node_start2()
{
	mynode->StartNode();
}
	
int si2p_get_node_state()
{
	return mynode->GetNodeState();

}