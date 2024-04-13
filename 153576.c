passthrough(struct readbuffer *obuf, char *str, int back)
{
    int cmd;
    Str tok = Strnew();
    char *str_bak;

    if (back) {
	Str str_save = Strnew_charp(str);
	Strshrink(obuf->line, obuf->line->ptr + obuf->line->length - str);
	str = str_save->ptr;
    }
    while (*str) {
	str_bak = str;
	if (sloppy_parse_line(&str)) {
	    char *q = str_bak;
	    cmd = gethtmlcmd(&q);
	    if (back) {
		struct link_stack *p;
		for (p = link_stack; p; p = p->next) {
		    if (p->cmd == cmd) {
			link_stack = p->next;
			break;
		    }
		}
		back = 0;
	    }
	    else {
		Strcat_charp_n(tok, str_bak, str - str_bak);
		push_tag(obuf, tok->ptr, cmd);
		Strclear(tok);
	    }
	}
	else {
	    push_nchars(obuf, 0, str_bak, str - str_bak, obuf->prev_ctype);
	}
    }
}