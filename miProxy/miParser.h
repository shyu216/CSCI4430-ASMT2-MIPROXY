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

// #include "DNSHeader.h"
// #include "DNSQuestion.h"
// #include "DNSRecord.h"

// #include "miHandler.h"
// #include "miCalculator.h"
// #include "miChooser.h"
// #include "miLogger.h"
// #include "miClient.h"
// #include "miServer.h"
// #include "miParser.h"

#define MAXCLIENTS 30
#define MAXSIZE 400000

char *parse(char *buf, char *chunk)
{
    // Initialize parameters
    char request[MAXSIZE];
    strcpy(request, buf);

    char line_buf[MAXSIZE];
    int offset = 0;

    char tail[MAXSIZE];
    memset(tail, 0, MAXSIZE * sizeof(char));

    int nbytes;

    // method uri version
    char method[MAXSIZE];
    char uri[MAXSIZE];
    char version[MAXSIZE];
    memset(method, 0, MAXSIZE * sizeof(char));
    memset(uri, 0, MAXSIZE * sizeof(char));
    memset(version, 0, MAXSIZE * sizeof(char));

    // Parse request
    do
    {
        // CLear line buffer
        memset(line_buf, 0, MAXSIZE * sizeof(char));

        // Read a line
        nbytes = readline(buf, line_buf, sizeof(buf), offset);

        // Get method uri version, e.g. "GET /index.html HTTP/1.1"
        if (offset == 0)
        {
            sscanf(line_buf, "%s %s %s", method, uri, version);
        }
        // Store the tail
        else
        {
            strcat(remaining_request, line_buf);
        }

        // Update offset
        offset += nbytes + 1;

        // IS video chunk
        char *new_chunk;

        clock_t start_time;
        clock_t end_time;

        // Read and Get time
        start_time = clock();


        // 先写到这里
    } while (nbytes);
}