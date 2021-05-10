#ifndef _SOCKET_TCP_H
#define _SOCKET_TCP_H

#ifdef __cplusplus //而这一部分就是告诉编译器，如果定义了__cplusplus(即如果是cpp文件， 
extern "C"{ //因为cpp文件默认定义了该宏),则采用C语言方式进行编译

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
