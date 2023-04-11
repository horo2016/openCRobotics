#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>



#include "socket_tcp.h"

#define STM32_SERVER_PORT    5556
#define MAX_MSG_BUF            512




int new_server_tcp;
int server_tcp;
 int tcp_socket_destroy()
{

	close(new_server_tcp);	
	close(server_tcp);

}
int tcp_socket_server_prepare(void)
{
	// set socket's address information
	// 设置一个socket地址结构server_addr,代表服务器internet的地址和端口
	struct sockaddr_in   server_addr;
	server_addr.sin_family = AF_INET;//地址家族
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);//ip
	/* server_addr.sin_addr.s_addr = inet_addr("193.169.4.223"); */
	server_addr.sin_port = htons(STM32_SERVER_PORT);//port
	bzero(&server_addr.sin_zero, 8);

	// create a stream socket
	// 创建用于internet的流协议(TCP)socket，用server_socket代表服务器向客户端提供服务的接口
	printf("server socket!!!\n");
	server_tcp = socket(AF_INET, SOCK_STREAM, 0);
	if (server_tcp < 0)
	{
		printf("Create Socket Failed!\n");
		return -1;
	}
	printf("start ...bind server socket!!!\n");
	// 把socket和socket地址结构绑定
	if (-1 == bind(server_tcp, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		printf("Server Bind Port: %d Failed!\n", STM32_SERVER_PORT);
		return -1;
	}
	printf("start listen server socket.\n");
	// server_socket用于监听
	if (-1 == listen(server_tcp, 2))
	{
		printf("Server Listen Failed!\n");
		return -1;
	}
	printf("start server  accept socket.\n");

	struct sockaddr_in client_addr;
	socklen_t          length = sizeof(client_addr);

	printf("wait client connect!!!\n");

	new_server_tcp = accept(server_tcp, (struct sockaddr*)&client_addr, &length);
	if (new_server_tcp < 0)
	{
		printf("Server Accept Failed!\n");
		//break;
	}

	return 0;
}
void *Socket_Task_handle (void *arg)
{
	struct timeval tm;
	int num_bytes;
	uint8_t msg_buf[MAX_MSG_BUF] = { 0 } ;
	tcp_socket_server_prepare();
	printf("client conneted \n");
	while(1)
	{

	   if (0 < (num_bytes = recv (new_server_tcp, msg_buf, MAX_MSG_BUF, 0)))
	   {
		   
		   printf("%s\n",msg_buf);
		   cmd_vel_callback(msg_buf);
	   }else if(0 > num_bytes  ){
		 //  socket_destroy();
	   }
	   memset(msg_buf,0,MAX_MSG_BUF);
		
	}	
	tcp_socket_destroy();
}