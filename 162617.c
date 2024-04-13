int get_if_num(const char* cardstr)
{
    char *buffer;
    int if_count=0;

    buffer = (char*) malloc(sizeof(char)*1025);
    if (buffer == NULL) {
		return -1;
	}

    strncpy(buffer, cardstr, 1025);
    buffer[1024] = '\0';

    while( (strsep(&buffer, ",") != NULL) && (if_count < MAX_CARDS) )
    {
        if_count++;
    }

    free(buffer)

    return if_count;
}