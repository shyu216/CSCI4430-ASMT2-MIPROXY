// PARSE AND GENERATE REQUEST OF VIDEO

void parse(char *buf, char *request, int br)
{
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

    // Check uri
    if (!strstr(uri, "Seg") || !strstr(uri, "Frag"))
    {
        strcpy(request, buf);
    }
    else
    {
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
}