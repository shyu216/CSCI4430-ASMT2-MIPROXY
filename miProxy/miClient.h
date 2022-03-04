// MAKE CLIENT SOCKET IN PROXY FOR REAL SERVER

int make_client_sockaddr(struct sockaddr_in *addr, const char *hostname, int port)
{
    addr->sin_family = AF_INET;
    struct hostent *host = gethostbyname(hostname);
    if (host == nullptr)
    {
        // fprintf(stderr, "%s: unknown host\n", hostname);
        exit(0);
    }
    memcpy(&(addr->sin_addr), host->h_addr, host->h_length);
    addr->sin_port = htons((unsigned short int)port);

    return 0;
}

int make_client(const char *hostname, int port)
{
    // (1) Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        // perror("Error opening stream socket");
        exit(0);
    }

    // (2) Create a sockaddr_in to specify remote host and port
    struct sockaddr_in addr;
    if (make_client_sockaddr(&addr, hostname, port) == -1)
    {
        exit(0);
    }

    // (3) Connect to remote server
    if (connect(sockfd, (sockaddr *)&addr, sizeof(addr)) == -1)
    {
        // perror("Error connecting stream socket");
        exit(0);
    }

    // (4) Return
    return sockfd;
}