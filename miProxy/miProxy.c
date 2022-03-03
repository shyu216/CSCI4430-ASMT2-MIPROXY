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
#include "miServer.h"

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
    else if (argc == 7 && strcmp(argv[1], "--nodns") == 0)
    {
        // bonus part here.
        printf("We dont have bonus part.\n");
        return 1;
    }
    else
    {
        printf("Error: missing or extra arguments\n");
        return 1;
    }
}