#include "socket_util.h"
#include <stdbool.h>
#include <unistd.h>

void ProcessReceivingMessagesThread(int sock_fd);
void ProcessReceivingMessages(int sock_fd);

int main()
{
    int sock_fd = CreateTCPIPv4Socket();

    char* ipv4_address = getenv("IPV4_ADDRESS");

    struct sockaddr_in *address = CreateIPv4Address(ipv4_address, 2000);

    int result = connect(sock_fd, address, sizeof(*address));
    if (result == 0)
    {
        printf("Connection was successful.\n");
    }
    else
    {
        printf("Connection was NOT successful");
        return 0;
    }

    char *name = NULL;
    size_t name_size = 0;
    printf("Please enter your name: \n");
    ssize_t name_count = getline(&name, &name_size, stdin);
    name[name_count - 1] = 0; // gets rid of \n

    char *line = NULL;
    size_t line_size = 0;
    printf("Type away!\nType 'exit' to terminate session\n");

    ProcessReceivingMessagesThread(sock_fd);

    char buffer[1024];

    while (true)
    {

        ssize_t char_count = getline(&line, &line_size, stdin);
        line[char_count - 1] = 0;
        sprintf(buffer, "%s: %s", name, line);

        if (char_count > 0)
        {
            if (strcmp(line, "exit") == 0)
            {
                break;
            }
            ssize_t amount_sent = send(sock_fd, buffer, strlen(buffer), 0);
        }
    }

    printf("Connection closing with server\n");
    close(sock_fd);

    return 0;
}

void ProcessReceivingMessagesThread(int sock_fd)
{
    pthread_t id;
    pthread_create(&id, NULL, ProcessReceivingMessages, sock_fd);
}

void ProcessReceivingMessages(int sock_fd)
{
    char buffer[1024];
    while (true)
    {
        ssize_t amount_received = recv(sock_fd, buffer, 1024, 0);
        if (amount_received > 0)
        {
            buffer[amount_received] = 0;
            printf("%s\n", buffer);
        }

        if (amount_received == 0)
        {
            break;
        }
    }

    close(sock_fd);
}
