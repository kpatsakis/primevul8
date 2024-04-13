close_anchor(struct html_feed_environ *h_env, struct readbuffer *obuf)
{
    if (obuf->anchor.url) {
	int i;
	char *p = NULL;
	int is_erased = 0;

	for (i = obuf->tag_sp - 1; i >= 0; i--) {
	    if (obuf->tag_stack[i]->cmd == HTML_A)
		break;
	}
	if (i < 0 && obuf->anchor.hseq > 0 && Strlastchar(obuf->line) == ' ') {
	    Strshrink(obuf->line, 1);
	    obuf->pos--;
	    is_erased = 1;
	}

	if (i >= 0 || (p = has_hidden_link(obuf, HTML_A))) {
	    if (obuf->anchor.hseq > 0) {
		HTMLlineproc1(ANSP, h_env);
		set_space_to_prevchar(obuf->prevchar);
	    }
	    else {
		if (i >= 0) {
		    obuf->tag_sp--;
		    bcopy(&obuf->tag_stack[i + 1], &obuf->tag_stack[i],
			  (obuf->tag_sp - i) * sizeof(struct cmdtable *));
		}
		else {
		    passthrough(obuf, p, 1);
		}
		bzero((void *)&obuf->anchor, sizeof(obuf->anchor));
		return;
	    }
	    is_erased = 0;
	}
	if (is_erased) {
	    Strcat_char(obuf->line, ' ');
	    obuf->pos++;
	}

	push_tag(obuf, "</a>", HTML_N_A);
    }
    bzero((void *)&obuf->anchor, sizeof(obuf->anchor));
}