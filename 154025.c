feed_table(struct table *tbl, char *line, struct table_mode *mode,
	   int width, int internal)
{
    int i;
    char *p;
    Str tmp;
    struct table_linfo *linfo = &tbl->linfo;

    if (*line == '<' && line[1] && REALLY_THE_BEGINNING_OF_A_TAG(line)) {
	struct parsed_tag *tag;
	p = line;
	tag = parse_tag(&p, internal);
	if (tag) {
	    switch (feed_table_tag(tbl, line, mode, width, tag)) {
	    case TAG_ACTION_NONE:
		return -1;
	    case TAG_ACTION_N_TABLE:
		return 0;
	    case TAG_ACTION_TABLE:
		return 1;
	    case TAG_ACTION_PLAIN:
		break;
	    case TAG_ACTION_FEED:
	    default:
		if (parsedtag_need_reconstruct(tag))
		    line = parsedtag2str(tag)->ptr;
	    }
	}
	else {
	    if (!(mode->pre_mode & (TBLM_PLAIN | TBLM_INTXTA | TBLM_INSELECT |
				    TBLM_SCRIPT | TBLM_STYLE)))
		return -1;
	}
    }
    else {
	if (mode->pre_mode & (TBLM_DEL | TBLM_S))
	    return -1;
    }
    if (mode->caption) {
	Strcat_charp(tbl->caption, line);
	return -1;
    }
    if (mode->pre_mode & TBLM_SCRIPT)
	return -1;
    if (mode->pre_mode & TBLM_STYLE)
	return -1;
    if (mode->pre_mode & TBLM_INTXTA) {
	feed_textarea(line);
	return -1;
    }
    if (mode->pre_mode & TBLM_INSELECT) {
	feed_select(line);
	return -1;
    }
    if (!(mode->pre_mode & TBLM_PLAIN) &&
	!(*line == '<' && line[strlen(line) - 1] == '>') &&
	strchr(line, '&') != NULL) {
	tmp = Strnew();
	for (p = line; *p;) {
	    char *q, *r;
	    if (*p == '&') {
		if (!strncasecmp(p, "&amp;", 5) ||
		    !strncasecmp(p, "&gt;", 4) || !strncasecmp(p, "&lt;", 4)) {
		    /* do not convert */
		    Strcat_char(tmp, *p);
		    p++;
		}
		else {
		    int ec;
		    q = p;
		    switch (ec = getescapechar(&p)) {
		    case '<':
			Strcat_charp(tmp, "&lt;");
			break;
		    case '>':
			Strcat_charp(tmp, "&gt;");
			break;
		    case '&':
			Strcat_charp(tmp, "&amp;");
			break;
		    case '\r':
			Strcat_char(tmp, '\n');
			break;
		    default:
			r = conv_entity(ec);
			if (r != NULL && strlen(r) == 1 &&
			    ec == (unsigned char)*r) {
			    Strcat_char(tmp, *r);
			    break;
			}
		    case -1:
			Strcat_char(tmp, *q);
			p = q + 1;
			break;
		    }
		}
	    }
	    else {
		Strcat_char(tmp, *p);
		p++;
	    }
	}
	line = tmp->ptr;
    }
    if (!(mode->pre_mode & (TBLM_SPECIAL & ~TBLM_NOBR))) {
	if (!(tbl->flag & TBL_IN_COL) || linfo->prev_spaces != 0)
	    while (IS_SPACE(*line))
		line++;
	if (*line == '\0')
	    return -1;
	check_rowcol(tbl, mode);
	if (mode->pre_mode & TBLM_NOBR && mode->nobr_offset < 0)
	    mode->nobr_offset = tbl->tabcontentssize;

	/* count of number of spaces skipped in normal mode */
	i = skip_space(tbl, line, linfo, !(mode->pre_mode & TBLM_NOBR));
	addcontentssize(tbl, visible_length(line) - i);
	setwidth(tbl, mode);
	pushdata(tbl, tbl->row, tbl->col, line);
    }
    else if (mode->pre_mode & TBLM_PRE_INT) {
	check_rowcol(tbl, mode);
	if (mode->nobr_offset < 0)
	    mode->nobr_offset = tbl->tabcontentssize;
	addcontentssize(tbl, maximum_visible_length(line, tbl->tabcontentssize));
	setwidth(tbl, mode);
	pushdata(tbl, tbl->row, tbl->col, line);
    }
    else {
	/* <pre> mode or something like it */
	check_rowcol(tbl, mode);
	while (*line) {
	    int nl = FALSE;
	    if ((p = strchr(line, '\r')) || (p = strchr(line, '\n'))) {
		if (*p == '\r' && p[1] == '\n')
		    p++;
		if (p[1]) {
		    p++;
		    tmp = Strnew_charp_n(line, p - line);
		    line = p;
		    p = tmp->ptr;
		}
		else {
		    p = line;
		    line = "";
		}
		nl = TRUE;
	    }
	    else {
		p = line;
		line = "";
	    }
	    if (mode->pre_mode & TBLM_PLAIN)
		i = maximum_visible_length_plain(p, tbl->tabcontentssize);
	    else
		i = maximum_visible_length(p, tbl->tabcontentssize);
	    addcontentssize(tbl, i);
	    setwidth(tbl, mode);
	    if (nl)
		clearcontentssize(tbl, mode);
	    pushdata(tbl, tbl->row, tbl->col, p);
	}
    }
    return -1;
}