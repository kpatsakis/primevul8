visible_length(char *str)
{
    int len = 0, n, max_len = 0;
    int status = R_ST_NORMAL;
    int prev_status = status;
    Str tagbuf = Strnew();
    char *t, *r2;
    int amp_len = 0;

    while (*str) {
	prev_status = status;
	if (next_status(*str, &status)) {
#ifdef USE_M17N
	    len += get_mcwidth(str);
	    n = get_mclen(str);
	}
	else {
	    n = 1;
	}
#else
	    len++;
	}
#endif
	if (status == R_ST_TAG0) {
	    Strclear(tagbuf);
	    PUSH_TAG(str, n);
	}
	else if (status == R_ST_TAG || status == R_ST_DQUOTE
		 || status == R_ST_QUOTE || status == R_ST_EQL
		 || status == R_ST_VALUE) {
	    PUSH_TAG(str, n);
	}
	else if (status == R_ST_AMP) {
	    if (prev_status == R_ST_NORMAL) {
		Strclear(tagbuf);
		len--;
		amp_len = 0;
	    }
	    else {
		PUSH_TAG(str, n);
		amp_len++;
	    }
	}
	else if (status == R_ST_NORMAL && prev_status == R_ST_AMP) {
	    PUSH_TAG(str, n);
	    r2 = tagbuf->ptr;
	    t = getescapecmd(&r2);
	    if (!*r2 && (*t == '\r' || *t == '\n')) {
		if (len > max_len)
		    max_len = len;
		len = 0;
	    }
	    else
		len += get_strwidth(t) + get_strwidth(r2);
	}
	else if (status == R_ST_NORMAL && ST_IS_REAL_TAG(prev_status)) {
	    ;
	}
	else if (*str == '\t') {
	    len--;
	    do {
		len++;
	    } while ((visible_length_offset + len) % Tabstop != 0);
	}
	else if (*str == '\r' || *str == '\n') {
	    len--;
	    if (len > max_len)
		max_len = len;
	    len = 0;
	}
#ifdef USE_M17N
	str += n;
#else
	str++;
#endif
    }