getMetaRefreshParam(char *q, Str *refresh_uri)
{
    int refresh_interval;
    char *r;
    Str s_tmp = NULL;

    if (q == NULL || refresh_uri == NULL)
	return 0;

    refresh_interval = atoi(q);
    if (refresh_interval < 0)
	return 0;

    while (*q) {
	if (!strncasecmp(q, "url=", 4)) {
	    q += 4;
	    if (*q == '\"' || *q == '\'')	/* " or ' */
		q++;
	    r = q;
	    while (*r && !IS_SPACE(*r) && *r != ';')
		r++;
	    s_tmp = Strnew_charp_n(q, r - q);

	    if (s_tmp->ptr[s_tmp->length - 1] == '\"'          /* " */
	       || s_tmp->ptr[s_tmp->length - 1] == '\'') {     /* ' */
		s_tmp->length--;
		s_tmp->ptr[s_tmp->length] = '\0';
	    }
	    q = r;
	}
	while (*q && *q != ';')
	    q++;
	if (*q == ';')
	    q++;
	while (*q && *q == ' ')
	    q++;
    }
    *refresh_uri = s_tmp;
    return refresh_interval;
}