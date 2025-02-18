#include "socket_util.h"

int CreateTCPIPv4Socket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in *CreateIPv4Address(char *ip, int port)
{
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_port = htons(port);
    address->sin_family = AF_INET;

    if (strlen(ip) == 0)
    {
        address->sin_addr.s_addr = INADDR_ANY; //listens for any incoming address (on server side)
    }
    else
    {
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);
    }

    return address;
}