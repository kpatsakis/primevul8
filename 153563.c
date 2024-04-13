append_tags(struct readbuffer *obuf)
{
    int i;
    int len = obuf->line->length;
    int set_bp = 0;

    for (i = 0; i < obuf->tag_sp; i++) {
	switch (obuf->tag_stack[i]->cmd) {
	case HTML_A:
	case HTML_IMG_ALT:
	case HTML_B:
	case HTML_U:
	case HTML_I:
	case HTML_S:
	    push_link(obuf->tag_stack[i]->cmd, obuf->line->length, obuf->pos);
	    break;
	}
	Strcat_charp(obuf->line, obuf->tag_stack[i]->cmdname);
	switch (obuf->tag_stack[i]->cmd) {
	case HTML_NOBR:
	    if (obuf->nobr_level > 1)
		break;
	case HTML_WBR:
	    set_bp = 1;
	    break;
	}
    }
    obuf->tag_sp = 0;
    if (set_bp)
	set_breakpoint(obuf, obuf->line->length - len);
}