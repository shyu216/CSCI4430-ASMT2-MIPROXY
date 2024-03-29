
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

#define MAXCLIENTNUM 16
#define HEADERLEN 102400
#define CONTENTLEN 1000000

// MAKE SERVER SOCKET IN PROXY FOR REAL CLIENT BROWSER
// (1)
int make_server_sockaddr(struct sockaddr_in *addr, int port)
{
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons((unsigned short int)port);

    return 0;
}
// (2)
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
        // perror("Error making server socket address");
        exit(0);
    }

    // (3b) Bind to the port
    if (bind(sockfd, (sockaddr *)&addr, sizeof(addr)) == -1)
    {
        // perror("Error binding stream socket");
        exit(0);
    }

    // (4) Begin listening for incoming connections
    if (listen(sockfd, MAXCLIENTNUM) == -1)
    {
        // perror("Error listening connection");
        exit(0);
    }

    // (5) Return
    // printf("Successfully make server, socket: %d\n", sockfd);
    return sockfd;
}

// MAKE CLIENT SOCKET IN PROXY FOR REAL SERVER
// (1)
int make_client_sockaddr(struct sockaddr_in *addr, const char *hostname, int port)
{
    addr->sin_family = AF_INET;
    struct hostent *host = gethostbyname(hostname);
    if (host == NULL)
    {
        return -1;
    }
    memcpy(&(addr->sin_addr), host->h_addr, host->h_length);
    addr->sin_port = htons((unsigned short int)port);

    return 0;
}
// (2)
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
        // perror("Error making client socket address");
        exit(0);
    }

    // (3) Connect to remote server
    if (connect(sockfd, (sockaddr *)&addr, sizeof(addr)) == -1)
    {
        // perror("Error connecting stream socket");
        exit(0);
    }

    // (4) Return
    // printf("Successfully make client, socket: %d\n", sockfd);
    return sockfd;
}

// CHOOSE A BITRATE
// (1)
void sort_bitrate(int *list, int len)
{
    for (int i = 0; i < len; i++)
    {
        for (int j = i + 1; j < len; ++j)
        {
            if (list[i] > list[j])
            {
                int temp = list[j];
                list[j] = list[i];
                list[i] = temp;
            }
        }
    }
}
// (2)
int choose_bitrate(double T_cur, int *list, int len)
{
    int i = 0;
    for (; i < len; ++i)
    {
        if (T_cur < 1.5 * list[i])
        {
            break;
        }
    }
    --i;
    i = i < 0 ? 0 : i;

    // printf("Bitrate %d is chosen\n", list[i]);
    return list[i];
}

// Read a line from buffer, Return the length read

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

// Handler

int handler(int listen_port, char *www_ip, double alpha, char *filename)
{
    /*
    (1) Preparation
    */

    // Open log file
    FILE *logFile;
    logFile = fopen(filename, "w+");
    if (logFile == NULL)
    {
        // perror("Error creating/opening file");
        exit(0);
    }
    fclose(logFile);

    // Initialize proxy socket
    int proxy_cli_fd;
    int proxy_ser_fd = make_server(listen_port);

    // Initialize client socket
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    int clientfd;
    int client_list[MAXCLIENTNUM] = {0};

    // Initialize socket descriptors
    fd_set fds;

    // Initialize throughput
    double T_cur = 0.001;
    double T_new = 0.001;

    // Initialize bitrate
    int br_list[HEADERLEN]; // THE SAME AS int *br_list=(int*)malloc(sizeof(int)*HEADERLEN);
    int br_len = 0;
    int br = 0;

    // Listen forever
    while (1)
    {
        // printf("Loop forever\n");

        // Clear and Add socket to set
        FD_ZERO(&fds);
        FD_SET(proxy_ser_fd, &fds);
        for (int i = 0; i < MAXCLIENTNUM; ++i)
        {
            if (client_list[i] > 0)
            {
                FD_SET(client_list[i], &fds);
            }
        }

        // Select
        if (select(FD_SETSIZE, &fds, NULL, NULL, NULL) == -1 && errno != EINTR)
        {
            // perror("select error");
            continue;
        }

        for (int i = 0; i < MAXCLIENTNUM; ++i)
        {
            if (FD_ISSET(i, &fds))
            {
                /*
                (2) Accept new connections from browsers
                */
                if (i == proxy_ser_fd)
                {
                    // printf("Waiting connection\n");

                    // Accept
                    clientfd = accept(proxy_ser_fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
                    if (clientfd == -1)
                    {
                        // perror("accept error");
                        continue;
                    }
                    else
                    {
                        // Add client to set
                        for (int j = 0; j < MAXCLIENTNUM; ++j)
                        {
                            if (client_list[j] == 0)
                            {
                                client_list[j] = clientfd;
                                break;
                            }
                        }

                        // Print info
                        // printf("\n---New client connection---\n");
                        // printf("socket fd is %d , ip is : %s , port : %d \n", clientfd, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                    }
                }

                /*
                (3) Handle a request
                */
                else
                {
                    // printf("\nStart to handle a request..., socket is %d\n", i);

                    proxy_cli_fd = make_client(www_ip, 80);

                    // Buffer of request&response
                    char buf[CONTENTLEN];
                    memset(buf, 0, CONTENTLEN * sizeof(char));
                    int nbytes;

                    // Revised request
                    char request[HEADERLEN];
                    memset(request, 0, HEADERLEN * sizeof(char));

                    // Buffer of line
                    char line_buf[HEADERLEN];
                    memset(line_buf, 0, HEADERLEN * sizeof(char));
                    int offset = 0;

                    // HTTP Header: method uri version, e.g. "GET /index.html HTTP/1.1"
                    char method[HEADERLEN];
                    char uri[HEADERLEN];
                    char version[HEADERLEN];
                    memset(method, 0, HEADERLEN * sizeof(char));
                    memset(uri, 0, HEADERLEN * sizeof(char));
                    memset(version, 0, HEADERLEN * sizeof(char));

                    // Part of uri
                    char path[HEADERLEN];
                    char chunk[HEADERLEN];
                    memset(path, 0, HEADERLEN * sizeof(char));
                    memset(chunk, 0, HEADERLEN * sizeof(char));

                    // Tail of request
                    char tail[HEADERLEN];
                    memset(tail, 0, HEADERLEN * sizeof(char));

                    /*
                    (1) Recv the request
                    */

                    nbytes = (int)recv(i, buf, sizeof(buf), 0);

                    // printf("Recv the request, nbytes: %d\n", nbytes);
                    // printf("###################################################################\n\n");
                    // printf("%.*s", nbytes, buf);
                    // printf("###################################################################\n\n");

                    if (nbytes < 1)
                    {
                        // Close the socket and Clear fd
                        // perror("Error receiving request");
                        close(i);
                        for (int j = 0; j < MAXCLIENTNUM; ++j)
                        {
                            if (client_list[j] == i)
                            {
                                client_list[j] = 0;
                                break;
                            }
                        }
                        close(proxy_cli_fd);
                        continue;
                    }

                    // Parse content type
                    nbytes = readline(buf, line_buf, sizeof(buf), offset);
                    offset += nbytes + 1; // Add 1 for "\n"
                    sscanf(line_buf, "%s %s %s\r\n", method, uri, version);

                    // printf("method: %s, uri: %s, version: %s\n", method, uri, version);

                    // Read the tail
                    while (nbytes)
                    {
                        memset(line_buf, 0, HEADERLEN * sizeof(char));
                        nbytes = readline(buf, line_buf, sizeof(buf), offset);
                        strcat(tail, line_buf);

                        offset += nbytes + 1;
                    }

                    // Read the last /r/n
                    nbytes = readline(buf, line_buf, sizeof(buf), offset);
                    strcat(tail, line_buf);

                    // printf("Finish read tail, result:\n%s\n", tail);

                    // IS video chunk
                    if (strstr(uri, "Seg") && strstr(uri, "Frag"))
                    {
                        // Choose bit rate
                        br = choose_bitrate(T_cur, br_list, br_len);

                        // Revise bitrate
                        sscanf(uri, "%[^0-9]%*d%s", path, chunk);
                        sprintf(line_buf, "%s%d%s", path, br, chunk);
                        sprintf(request, "%s %s %s\r\n", method, line_buf, version);
                        strcat(request, tail);
                    }
                    // IS document chunk
                    else
                    {
                        strcpy(request, buf);
                    }

                    /*
                    (2) Send revised request
                    */

                    // Send
                    // printf("Send...");
                    nbytes = (int)send(proxy_cli_fd, request, sizeof(request), 0);
                    if (nbytes == -1)
                    {
                        // perror("Error sending request message");
                        close(proxy_cli_fd);
                        break;
                    }

                    // printf(", Done, nbytes is %d\n", nbytes);
                    // printf("###################################################################\n\n");
                    // printf("%s", request);
                    // printf("###################################################################\n\n");

                    // Clean buf
                    memset(buf, 0, CONTENTLEN * sizeof(char));

                    // Make sure server will reply // 为森么要这样子server才会回啊？？？
                    strcat(buf, "\r\n");
                    nbytes = (int)send(proxy_cli_fd, buf, 2 * sizeof(char), 0);
                    memset(buf, 0, CONTENTLEN * sizeof(char));
                    
                    // printf("Send only \\r\\n ..., nbytes is %d\n", nbytes);
                    // printf("###################################################################\n\n");
                    // printf("%s", buf);
                    // printf("###################################################################\n\n");

                    /*
                    (3) Recv response
                    */

                    // Start timing
                    // printf("Start timing...");
                    // clock_t start, end;
                    struct timeval start;
                    struct timeval end;

                    gettimeofday(&start, NULL);
                    // printf("start time: %ld, Done\n", start);

                    // Recv first chunk
                    // printf("Recv first chunk...");
                    nbytes = (int)recv(proxy_cli_fd, buf, HEADERLEN * sizeof(char), 0);
                    if (nbytes == -1)
                    {
                        // perror("Error receiving response");
                        close(proxy_cli_fd);
                        break;
                    }

                    // printf(", Done, nbytes is %d\n", nbytes);

                    // Content length parameters
                    int first_read = nbytes;
                    int remain_to_read;
                    int content_length = 0;

                    // Parse content length
                    offset = 0;
                    // printf("Parse content length\n");
                    while (strcmp(line_buf, "\r\n"))
                    {
                        memset(line_buf, 0, HEADERLEN * sizeof(char));
                        nbytes = readline(buf, line_buf, sizeof(line_buf), offset);

                        char *cl_addr = strstr(line_buf, "Content-Length: ");
                        if (cl_addr)
                        {
                            sscanf(cl_addr, "Content-Length: %d", &content_length);
                        }

                        offset += nbytes + 1;
                    }

                    // printf("###################################################################\n\n");
                    // printf("%.*s", offset, buf);
                    // printf("###################################################################\n\n");

                    // Recv (continue)
                    remain_to_read = content_length - (first_read - offset);

                    // printf("Header length: %d\n", offset);
                    // printf("Content-Length: %d\n", content_length);
                    // printf("Remain to read: %d\n", remain_to_read);
                    int totallen = remain_to_read + first_read;

                    char *buf_ptr = buf + first_read;
                    while (remain_to_read > 0)
                    {
                        nbytes = (int)recv(proxy_cli_fd, buf_ptr, remain_to_read, 0);
                        if (nbytes == -1)
                        {
                            // perror("Error receiving response");
                            close(proxy_cli_fd);
                            break;
                        }
                        remain_to_read -= nbytes;
                        buf_ptr += nbytes;
                    }

                    // nbytes = readline(buf, line_buf, sizeof(buf), offset);
                    // strcat(tail, line_buf);

                    // printf("Remain to read: %d\n", remain_to_read);

                    // IS Video meta
                    if (strstr(uri, ".f4m"))
                    {
                        // printf("IS Video meta\n");

                        // Get bitrate
                        offset = 0;
                        while (!strstr(line_buf, "</manifest>"))
                        {
                            memset(line_buf, 0, HEADERLEN * sizeof(char));
                            nbytes = readline(buf, line_buf, sizeof(line_buf), offset);

                            offset += nbytes + 1;

                            // Find bitrate address
                            char *br_addr = strstr(line_buf, "bitrate");
                            if (br_addr)
                            {
                                sscanf(br_addr, "bitrate=\"%d\"", &br_list[br_len]);

                                // printf("Add bitrate: %d\n", br_list[br_len]);
                                ++br_len;
                            }
                        }

                        // Sort bitrate
                        sort_bitrate(br_list, br_len);

                        // Set T_cur
                        T_cur = br_list[0];
                    }

                    // IS Video chunk
                    else if (strstr(uri, "Seg") && strstr(uri, "Frag"))
                    {
                        // printf("IS Video chunk\n");

                        // ENd timting
                        gettimeofday(&end, NULL);
                        double stamp = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000;

                        // Calculate time

                        // printf("End: %d", (int)end);
                        // printf("Here, timestamp: %lf, T_new: %lf, T_cur: %lf\n", stamp, T_new, T_cur);
                        // T_new = content_length / 1000 / stamp; // kbps

                        // Bytes * 8 / 1000 / sec
                        T_new = (double)((totallen * 8) / 1000) / stamp;
                        T_cur = alpha * T_new + (1 - alpha) * T_cur;
                        // printf("Then, timestamp: %lf, T_new: %lf, T_cur: %lf\n", stamp, T_new, T_cur);

                        // Get client ip
                        getpeername(i, (struct sockaddr *)&addr, (socklen_t *)&addrlen);

                        // Open file
                        FILE *logFile;
                        logFile = fopen(filename, "a+");
                        if (logFile == NULL)
                        {
                            perror("Error creating/opening file");
                            close(proxy_cli_fd);
                            continue;
                        }

                        fprintf(logFile, "%s %d%s %s %lf %lf %lf %d\n", inet_ntoa(addr.sin_addr), br, chunk, www_ip, stamp, T_new, T_cur, br);
                        // printf("Generate log: %s %d%s %s %lf %lf %lf %d\n", inet_ntoa(addr.sin_addr), br, chunk, www_ip, stamp, T_new, T_cur, br);

                        fclose(logFile);
                    }

                    else
                    {
                        // printf("IS something else\n");
                    }

                    /*
                    (4) Send response
                    */

                    // Send
                    nbytes = (int)send(i, buf, sizeof(buf), 0);
                    if (nbytes == -1)
                    {
                        // perror("Error sending response");
                        close(proxy_cli_fd);
                        continue;
                    }

                    // Close
                    close(proxy_cli_fd);
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc == 6 && strcmp(argv[1], "--nodns") == 0)
    {
        int listen_port = atoi(argv[2]);
        char *www_ip = argv[3];
        double alpha = atof(argv[4]);
        char *log_file = argv[5];

        return handler(listen_port, www_ip, alpha, log_file);
    }
    else if (argc == 7 && strcmp(argv[1], "--dns") == 0)
    {
        // bonus part here.
        // printf("We dont have bonus part.\n");
        // printf("if you get the ip, you can return handler(listen_port, www_ip, alpha, log_file);\n");
        return 1;
    }
    else
    {
        // printf("Error: missing or extra arguments\n");
        return 1;
    }
}