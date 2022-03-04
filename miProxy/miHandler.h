// HANDLE ALL THE THINGS

int readline(char *src, char *dst, int maxlen, int offset)
{
    int n;
    char *temp = src + offset;

    for (n = 0; n < maxlen; n++)
    {
        dst[n] = temp[n];
        if (temp[n] == '\n' || temp[n] == '\0')
        {
            break;
        }
    }
    return n;
}

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
    int br_list[HEADERLEN]; // THE SAME AS int *br_list=(int*)malloc(sizeof(int)*HEADERLEN);
    int br_len = 0;

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
                    char buf[CONTENTLEN];
                    char line_buf[HEADERLEN];

                    memset(buf, 0, CONTENTLEN * sizeof(char));
                    ssize_t nbytes;

                    printf("\n\n###################################################################\n\n");

                    nbytes = recv(i, buf, sizeof(buf), 0);
                    printf("%s", buf);

                    printf("\n\n###################################################################\n\n");
                    // nbytes = send(proxyfd2, buf, sizeof(buf), 0);
                    // nbytes = recv(proxyfd2, buf, sizeof(buf), 0);
                    // nbytes = send(i, buf, sizeof(buf), 0);
                    // printf("%s", buf);

                    if (nbytes == -1)
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
                        char request[HEADERLEN];
                        memset(request, 0, HEADERLEN * sizeof(char));

                        // Get request from client
                        char line_buf[HEADERLEN];
                        memset(line_buf, 0, HEADERLEN * sizeof(char));
                        int offset = 0;

                        // Get method uri version, e.g. "GET /index.html HTTP/1.1"
                        char method[HEADERLEN];
                        char uri[HEADERLEN];
                        char version[HEADERLEN];
                        memset(method, 0, HEADERLEN * sizeof(char));
                        memset(uri, 0, HEADERLEN * sizeof(char));
                        memset(version, 0, HEADERLEN * sizeof(char));

                        char path[HEADERLEN];
                        char chunk[HEADERLEN];

                        int nbytes = readline(buf, line_buf, sizeof(buf), offset);
                        sscanf(line_buf, "%s %s %s", method, uri, version);

                        // IS video chunk
                        if (strstr(uri, "Seg") && strstr(uri, "Frag"))
                        {
                            // Choose bit rate
                            sort_bitrate(br_list, br_len);
                            int br = choose_bitrate(T_cur, br_list, br_len);

                            // Revise bitrate
                            sscanf(uri, "%[^0-9]%*d%s", path, chunk);
                            sprintf(line_buf, "%s%d%s", path, br, chunk);
                            sprintf(request, "%s %s %s\r\n", method, line_buf, version);

                            // Store Remaining buf
                            while (nbytes)
                            {
                                memset(line_buf, 0, HEADERLEN * sizeof(char));
                                nbytes = readline(buf, line_buf, sizeof(buf), offset);
                                strcat(request, line_buf);

                                // Add 1 for "\n"
                                offset += nbytes + 1;
                            }
                        }
                        else
                        {
                            strcpy(request, buf);

                            // IS video meta
                        }

                        // Send to server and recv
                        nbytes = send(proxyfd2, request, HEADERLEN * sizeof(char), 0);
                        if (nbytes == -1)
                        {
                            // perror("Error sending request message");
                            exit(0);
                        }

                        // Initialize buf and line buf
                        memset(buf, 0, CONTENTLEN * sizeof(buf));

                        // Start timing
                        clock_t start, end;
                        start = clock();

                        // Recv first chunk, probably the header
                        nbytes = recv(proxyfd2, buf, sizeof(buf), 0);
                        if (nbytes == -1)
                        {
                            // perror("Error streaming video");
                            exit(0);
                        }

                        // Content length
                        int first_read = nbytes;
                        int remain_read;
                        int content_length = 0;

                        // Get content length
                        int offset = 0;
                        while (nbytes)
                        {
                            memset(line_buf, 0, HEADERLEN * sizeof(char));
                            nbytes = readline(buf, line_buf, sizeof(line_buf), offset);

                            if (strstr(line_buf, "Content-Length: "))
                            {
                                sscanf(line_buf, "Content-Length: %d", &content_length);
                            }

                            offset += nbytes + 1;

                            if (strcmp(line_buf, "\r\n\r\n") == 0)
                            {
                                break;
                            }
                        }

                        // Keep reading
                        remain_read = content_length - (first_read - offset);
                        char *buf_ptr = buf + first_read;
                        while (remain_read)
                        {
                            nbytes = recv(proxyfd2, buf_ptr, remain_read * sizeof(char), 0);
                            if (nbytes == -1)
                            {
                                // perror("Error streaming video");
                                exit(0);
                            }
                            remain_read -= nbytes;
                            buf_ptr += nbytes;
                        }

                        // IS Video chunk
                        if (strstr(buf, "Seg") && strstr(buf, "Frag"))
                        {
                            // ENd timting
                            end = clock;
                            double stamp = (end - start) / CLOCKS_PER_SEC;

                            // Calculate time
                            T_new = content_length / (stamp * 1000);
                            T_cur = alpha * T_new + (1 - alpha) * T_cur;
                        }

                        // IS Video meta
                        if (strstr(buf, ".f4m"))
                        {
                            offset = 0;
                        }
                    }
                }
            }
        }
    }
}