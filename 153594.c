is_text_type(char *type)
{
    return (type == NULL || type[0] == '\0' ||
	    strncasecmp(type, "text/", 5) == 0 ||
	    (strncasecmp(type, "application/", 12) == 0 &&
		strstr(type, "xhtml") != NULL) ||
	    strncasecmp(type, "message/", sizeof("message/") - 1) == 0);
}