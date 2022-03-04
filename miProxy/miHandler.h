// HANDLE ALL THE THINGS

int handler(int listen_port, char *www_ip, double alpha, char *log_file)
{
    // Initialize proxy socket
    int proxyfd2 = make_client(www_ip, 80);
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
    double T_new = 0.0;

    // Initialize bitrate
    int br_list[HEADERLEN];
    int br_index = 0;

    // Listen forever
    while (1)
    {
        // Select
        if (select(FD_SETSIZE, &fds, NULL, NULL, NULL) == -1 && errno != EINTR)
        {
            perror("select error");
        }

        for (int i = 0; i < MAXCLIENTNUM; ++i)
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
                    char buf[HEADERLEN];
                    memset(buf, 0, HEADERLEN * sizeof(char));
                    ssize_t nbytes = recv(i, buf, sizeof(buf), 0);

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
                        // Choose bit rate
                        int br = choose_bitrate(T_cur, br_list, br_index);

                        // Parse buf and generate request
                        char request[HEADERLEN];
                        memset(request, 0, HEADERLEN * sizeof(char));
                        parse(buf, request, br);

                        // Send to server and recv

                        // Calculate time
                        T_cur = calculate(T_cur, T_new, alpha);

                        // Send to client
                    }
                }
            }
        }
    }
}