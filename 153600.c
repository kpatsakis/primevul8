check_accept_charset(char *ac)
{
    char *s = ac, *e;

    while (*s) {
	while (*s && (IS_SPACE(*s) || *s == ','))
	    s++;
	if (!*s)
	    break;
	e = s;
	while (*e && !(IS_SPACE(*e) || *e == ','))
	    e++;
	if (wc_guess_charset(Strnew_charp_n(s, e - s)->ptr, 0))
	    return ac;
	s = e;
    }
    return NULL;
}