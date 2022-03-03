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

// #include "miHandler.h"
#include "miCalculator.h"
#include "miChooser.h"
#include "miLogger.h"
#include "miClient.h"
#include "miServer.h"
#include "miParser.h"

#define MAXCLIENTS 30
#define MAXSIZE 400000

int handler(int listen_port, char *www_ip, double alpha, char *log_file)
{
    // Initialize proxy socket
    char server_port[MAXSIZE] = "80";
    int proxyfd2 = make_client(www_ip, server_port);
    int proxyfd = make_server(listen_port);

    // Initialize client socket
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    int clientfd;

    // Initialize socket descriptors
    fd_set fds;

    // Clear and Add proxy to set
    FD_ZERO(&fds);
    FD_SET(proxyfd, &fds);

    // Initialize throughput
    double T_cur = 0.0;
    douvle T_new = 0.0;

    // Listen forever
    while (1)
    {
        // Select
        if (select(FD_SETSIZE, &fds, NULL, NULL, NULL) == -1 && errno != EINTR)
        {
            perror("select error");
        }

        for (int i = 0; i < MAXCLIENTS; ++i)
        {
            if (FD_ISSET(i, &fds))
            {

                // Listen new connections
                if (i == proxyfd)
                {
                    // Accept
                    clientfd = accept(proxyfd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
                    if (clientfd == -1)
                    {
                        perror("accept error");
                    }
                    else
                    {
                        // Add client to set
                        FD_SET(clientfd, &fds);

                        // Somebody connected, get their details and print
                        printf("\n---New client connection---\n");
                        printf("socket fd is %d , ip is : %s , port : %d \n", clientfd, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                    }
                }
                // Handle old connections
                else
                {
                    char buf[MAXSIZE];
                    int nbytes = read(i, buf, sizeof(buf));

                    if (nbytes == 0)
                    {
                        // Somebody disconnected, get their details and print
                        printf("\n---Client disconnected---\n");
                        printf("socket ip is : %s , port : %d \n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

                        // Close the socket and Clear fd
                        close(i);
                        FD_CLR(i, &fds);
                    }
                    else
                    {
                        // Parse buf and generate request
                        char *request = parse(buf, T_cur);
                    }
                }
            }
        }
    }
}