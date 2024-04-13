skip_space(struct table *t, char *line, struct table_linfo *linfo,
	   int checkminimum)
{
    int skip = 0, s = linfo->prev_spaces;
    Lineprop ctype, prev_ctype = linfo->prev_ctype;
    Str prevchar = linfo->prevchar;
    int w = linfo->length;
    int min = 1;

    if (*line == '<' && line[strlen(line) - 1] == '>') {
	if (checkminimum)
	    check_minimum0(t, visible_length(line));
	return 0;
    }

    while (*line) {
	char *save = line, *c = line;
	int ec, len, wlen, plen;
	ctype = get_mctype(line);
	len = get_mcwidth(line);
	wlen = plen = get_mclen(line);

	if (min < w)
	    min = w;
	if (ctype == PC_ASCII && IS_SPACE(*c)) {
	    w = 0;
	    s++;
	}
	else {
	    if (*c == '&') {
		ec = getescapechar(&line);
		if (ec >= 0) {
		    c = conv_entity(ec);
		    ctype = get_mctype(c);
		    len = get_strwidth(c);
		    wlen = line - save;
		    plen = get_mclen(c);
		}
	    }
	    if (prevchar->length && is_boundary((unsigned char *)prevchar->ptr,
						(unsigned char *)c)) {
		w = len;
	    }
	    else {
		w += len;
	    }
	    if (s > 0) {
#ifdef USE_M17N
		if (ctype == PC_KANJI1 && prev_ctype == PC_KANJI1)
		    skip += s;
		else
#endif
		    skip += s - 1;
	    }
	    s = 0;
	    prev_ctype = ctype;
	}
	set_prevchar(prevchar, c, plen);
	line = save + wlen;
    }
    if (s > 1) {
	skip += s - 1;
	linfo->prev_spaces = 1;
    }
    else {
	linfo->prev_spaces = s;
    }
    linfo->prev_ctype = prev_ctype;
    linfo->prevchar = prevchar;

    if (checkminimum) {
	if (min < w)
	    min = w;
	linfo->length = w;
	check_minimum0(t, min);
    }
    return skip;
}