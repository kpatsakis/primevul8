qstr_unquote(Str s)
{
    char *p;

    if (s == NULL)
	return NULL;
    p = s->ptr;
    if (*p == '"') {
	Str tmp = Strnew();
	for (p++; *p != '\0'; p++) {
	    if (*p == '\\')
		p++;
	    Strcat_char(tmp, *p);
	}
	if (Strlastchar(tmp) == '"')
	    Strshrink(tmp, 1);
	return tmp;
    }
    else
	return s;
}