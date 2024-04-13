checkContentType(Buffer *buf)
{
    char *p;
    Str r;
    p = checkHeader(buf, "Content-Type:");
    if (p == NULL)
	return NULL;
    r = Strnew();
    while (*p && *p != ';' && !IS_SPACE(*p))
	Strcat_char(r, *p++);
#ifdef USE_M17N
    if ((p = strcasestr(p, "charset")) != NULL) {
	p += 7;
	SKIP_BLANKS(p);
	if (*p == '=') {
	    p++;
	    SKIP_BLANKS(p);
	    if (*p == '"')
		p++;
	    content_charset = wc_guess_charset(p, 0);
	}
    }
#endif
    return r->ptr;
}