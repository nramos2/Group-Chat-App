#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>

struct sockaddr_in *CreateIPv4Address(char *ip, int port);

int CreateTCPIPv4Socket();