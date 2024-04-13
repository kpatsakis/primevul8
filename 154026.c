    if (status == R_ST_AMP) {
	r2 = tagbuf->ptr;
	t = getescapecmd(&r2);
	if (*t != '\r' && *t != '\n')
	    len += get_strwidth(t) + get_strwidth(r2);
    }