#ifndef _SOCKET_TCP_H
#define _SOCKET_TCP_H

#ifdef __cplusplus //����һ���־��Ǹ��߱����������������__cplusplus(�������cpp�ļ��� 
extern "C"{ //��Ϊcpp�ļ�Ĭ�϶����˸ú�),�����C���Է�ʽ���б���

#endif

#include "stdio.h"

extern int new_server_tcp;
extern int server_tcp;
extern int tcp_socket_server_prepare(void);
extern int tcp_socket_destroy();


extern void *Socket_Task_handle (void *arg);
#ifdef __cplusplus
}
#endif
#endif
