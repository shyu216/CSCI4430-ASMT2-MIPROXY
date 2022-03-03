#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include "DNSHeader.h"
#include "DNSQuestion.h"
#include "DNSRecord.h"

#include "miHandler.h"
#include "miCalculator.h"
#include "miChooser.h"
#include "miLogger.h"
#include "miClient.h"
// #include "miServer.h"
#include "miParser.h"

int make_server_sockaddr(struct sockaddr_in *addr, int port)
{
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons((unsigned short int)port);

    return 0;
}

int make_server(int port)
{
    // (1) Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        // perror("Error opening stream socket");
        exit(0);
    }

    // (2) Set the "reuse port" socket option
    int yesval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof(yesval)) == -1)
    {
        // perror("Error setting socket options");
        exit(0);
    }

    // (3) Create a sockaddr_in struct for the proper port and bind() to it
    struct sockaddr_in addr;
    if (make_server_sockaddr(&addr, port) == -1)
    {
        exit(0);
    }

    // (3b) Bind to the port
    if (bind(sockfd, (sockaddr *)&addr, sizeof(addr)) == -1)
    {
        // perror("Error binding stream socket");
        exit(0);
    }

    // (4) Begin listening for incoming connections
    if (listen(sockfd, MAXCLIENTS) == -1)
    {
        // perror("Error listening connection");
        exit(0);
    }

    // (5) Return
    return sockfd;
}
