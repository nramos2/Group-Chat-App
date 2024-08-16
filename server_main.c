#include "socket_util.h"
#include <stdbool.h>
#include <unistd.h>

struct AcceptedSocket
{
    int accepted_sock_fd;
    struct sockaddr_in address;
    int error;
    bool accepted_successfully;
};

struct AcceptedSocket *AcceptIncomingConnection(int socket);

void ReceiveAndProcessIncomingData(int sock_fd);
void ReceiveAndProcessIncomingDataThread(struct AcceptedSocket *p_socket);
void StartAcceptingConnections(int server_socket_fd);
void BroadcastMessageToOthers(char* buffer, int sock_fd);

struct AcceptedSocket accepted_sockets_[10];
int accepted_sockets_count_ = 0;

int main()
{
    int serv_sock_fd = CreateTCPIPv4Socket();

    struct sockaddr_in *serv_address = CreateIPv4Address("", 2000);

    int result = bind(serv_sock_fd, serv_address, sizeof(*serv_address));
    if (result == 0)
    {
        printf("Server socket was bound successfully.\n");
    }
    else
    {
        printf("Server socket was NOT bound successfully.\n");
    }

    int listen_result = listen(serv_sock_fd, 10); // n is backlog or amount of clients to take

    StartAcceptingConnections(serv_sock_fd);

    printf("Connection closing with client\n");
    shutdown(serv_sock_fd, SHUT_RDWR);

    return 0;
}

struct AcceptedSocket *AcceptIncomingConnection(int socket)
{
    struct sockaddr_in cli_address;
    int cli_address_size = sizeof(struct sockaddr_in);
    int cli_sock_fd = accept(socket, &cli_address, &cli_address_size);

    struct AcceptedSocket *accepted_sock = malloc(sizeof(struct AcceptedSocket));
    accepted_sock->address = cli_address;
    accepted_sock->accepted_sock_fd = cli_sock_fd;
    accepted_sock->accepted_successfully = cli_sock_fd > 0;

    if (!accepted_sock->accepted_successfully)
    {
        accepted_sock->error = cli_sock_fd;
    }

    return accepted_sock;
}

void ReceiveAndProcessIncomingData(int sock_fd)
{
    char buffer[1024];
    while (true)
    {
        ssize_t amount_received = recv(sock_fd, buffer, 1024, 0);
        if (amount_received > 0)
        {
            buffer[amount_received] = 0;
            printf("%s\n", buffer);

            BroadcastMessageToOthers(buffer, sock_fd);
        }

        if (amount_received == 0)
        {
            break;
        }
    }

    close(sock_fd);
}

void StartAcceptingConnections(int server_socket_fd)
{
    while (true)
    {
        struct AcceptedSocket *client_socket = AcceptIncomingConnection(server_socket_fd);
        accepted_sockets_[accepted_sockets_count_++] = *client_socket;
        ReceiveAndProcessIncomingDataThread(client_socket);
    }
}

void BroadcastMessageToOthers(char *buffer, int sock_fd)
{
    for(int i = 0; i < accepted_sockets_count_; i++)
    {
        if(accepted_sockets_[i].accepted_sock_fd != sock_fd)
        {
            send(accepted_sockets_[i].accepted_sock_fd, buffer, strlen(buffer), 0);
        }
    }
}

void ReceiveAndProcessIncomingDataThread(struct AcceptedSocket *p_socket)
{
    pthread_t id;
    pthread_create(&id, NULL, ReceiveAndProcessIncomingData, p_socket->accepted_sock_fd);
    
}
