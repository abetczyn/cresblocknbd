#include <stdio.h>
#include "tcpclient.h"
#include <string.h>

int main()
{
    const char *sendstring = "Hello, world!";
    printf("%lu\n", sizeof(size_t));
    return 0;
    int clientfd = -1;
    int ret = 0;
    char input = '\0';
    char buffer[13];
    buffer[12] = 0;
    while(input != 'q')
    {
        input = getc(stdin);
        switch(input)
        {
            case 'c':
                clientfd = client_connect("192.168.1.4", 60010);
                printf("Connected with code %d\n", clientfd);
            break;
            case 'd':
                client_disconnect(clientfd);
            break;
            case 's':
                ret = client_send(clientfd, sendstring, strlen(sendstring));
                printf("Sent with code %d\n", ret);
            break;
            case 'r':
                ret = client_receive(clientfd, buffer, 12);
                printf("Received with code %d\n", ret);
                if(ret == 0)
                {
                    printf("Buffer: %s\n", buffer);
                }
            break;
        }
    }
    
    return 0;
}