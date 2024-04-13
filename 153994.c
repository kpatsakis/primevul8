visible_length_plain(char *str)
{
    int len = 0, max_len = 0;

    while (*str) {
	if (*str == '\t') {
	    do {
		len++;
	    } while ((visible_length_offset + len) % Tabstop != 0);
	    str++;
	}
	else if (*str == '\r' || *str == '\n') {
	    if (len > max_len)
		max_len = len;
	    len = 0;
	    str++;
	}
	else {
#ifdef USE_M17N
	    len += get_mcwidth(str);
	    str += get_mclen(str);
#else
	    len++;
	    str++;
#endif
	}
    }
    return len > max_len ? len : max_len;
}