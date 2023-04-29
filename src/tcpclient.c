#include "tcpclient.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>

int client_send(int socketfd, const void* data, size_t dataSize)
{
    const char *cdata = data;
    while(dataSize > 0)
    {
        ssize_t sent = write(socketfd, cdata, dataSize);
        if(sent > 0)
        {
            dataSize -= sent;
            cdata += sent;
        }
        else
        {
            close(socketfd);
            return TCP_ERR_CLOSED;
        }
    }
    return 0;
}

int client_receive(int socketfd, void* data, size_t dataSize)
{
    char *cdata = data;
    while(dataSize > 0)
    {
        ssize_t received = read(socketfd, cdata, dataSize);
        if(received > 0)
        {
            dataSize -= received;
            cdata += received;
        }
        else
        {
            close(socketfd);
            return TCP_ERR_CLOSED;
        }
    }
    return 0;
}

void client_disconnect(int socketfd)
{
    close(socketfd);
}

int client_connect(const char *host, unsigned short port)
{
    //create socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd < 0)
    {
        return TCP_ERR_SOCKET_CREATE;
    }

    //connection information datastructure
    struct sockaddr_in serverAddress = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = inet_addr(host),
    };
    bzero(&serverAddress.sin_zero, 8);

    //connect
    if(connect(socketfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)))
    {
        close(socketfd);
        return TCP_ERR_SOCKET_CONNECT;
    }

    return socketfd;
}