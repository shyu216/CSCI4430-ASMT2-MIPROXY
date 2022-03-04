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

#define MAXCLIENTNUM 800
#define HEADERLEN 102400
#define CONTENTLEN 400000

#include "miHelper.h"
#include "miChooser.h"
#include "miLogger.h"
#include "miClient.h"
#include "miServer.h"
#include "miParser.h"
#include "miCalculator.h"
#include "miHandler.h"

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
        printf("if you get the ip, you can return handler(listen_port, www_ip, alpha, log_file);");
        return 1;
    }
    else
    {
        printf("Error: missing or extra arguments\n");
        return 1;
    }
}