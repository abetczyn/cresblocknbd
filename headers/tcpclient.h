#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__

#define TCP_ERR_SOCKET_CREATE -1
#define TCP_ERR_SOCKET_CONNECT -2
#define TCP_ERR_CLOSED -4

#include <stdlib.h>

int client_receive(int socketfd, void* data, size_t dataSize);
int client_send(int socketfd, const void* data, size_t dataSize);
int client_connect(const char *host, unsigned short port);
void client_disconnect(int socketfd);

#endif