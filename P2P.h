/*
 * SI2P, SIP based Peer-to-Peer Network Location System
 * Copyright 2005 MPRC
 * Edit by LiZang
 */
//0820

#ifndef _P2P_H_
#define _P2P_H_
#ifdef __cplusplus
extern "C"
{
#endif

int  si2p_register_user(char *aor); //aor="sip:liqq@sip.com"
int  si2p_query_user(char *aor,char **contact); //contact="sip:liqq@192.168.1.61"
int  si2p_p2p_init();
void  si2p_node_leave();
void  si2p_node_join();
void  si2p_node_start(bool asingle);
void si2p_node_start2();

int si2p_get_node_state();

void cmdloop();

#ifdef __cplusplus
}
#endif

#endif