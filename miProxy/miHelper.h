// readline from *char to *char

int readline(char *src, char *dst, int maxlen, int offset) 
{
    int n;
    char *temp = src+offset;

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