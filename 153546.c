feed_select(char *str)
{
    Str tmp = Strnew();
    int prev_status = cur_status;
    static int prev_spaces = -1;
    char *p;

    if (cur_select == NULL)
	return;
    while (read_token(tmp, &str, &cur_status, 0, 0)) {
	if (cur_status != R_ST_NORMAL || prev_status != R_ST_NORMAL)
	    continue;
	p = tmp->ptr;
	if (tmp->ptr[0] == '<' && Strlastchar(tmp) == '>') {
	    struct parsed_tag *tag;
	    char *q;
	    if (!(tag = parse_tag(&p, FALSE)))
		continue;
	    switch (tag->tagid) {
	    case HTML_OPTION:
		process_option();
		cur_option = Strnew();
		if (parsedtag_get_value(tag, ATTR_VALUE, &q))
		    cur_option_value = Strnew_charp(q);
		else
		    cur_option_value = NULL;
		if (parsedtag_get_value(tag, ATTR_LABEL, &q))
		    cur_option_label = Strnew_charp(q);
		else
		    cur_option_label = NULL;
		cur_option_selected = parsedtag_exists(tag, ATTR_SELECTED);
		prev_spaces = -1;
		break;
	    case HTML_N_OPTION:
		/* do nothing */
		break;
	    default:
		/* never happen */
		break;
	    }
	}
	else if (cur_option) {
	    while (*p) {
		if (IS_SPACE(*p) && prev_spaces != 0) {
		    p++;
		    if (prev_spaces > 0)
			prev_spaces++;
		}
		else {
		    if (IS_SPACE(*p))
			prev_spaces = 1;
		    else
			prev_spaces = 0;
		    if (*p == '&')
			Strcat_charp(cur_option, getescapecmd(&p));
		    else
			Strcat_char(cur_option, *(p++));
		}
	    }
	}
    }
}