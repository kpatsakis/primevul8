flushline(struct html_feed_environ *h_env, struct readbuffer *obuf, int indent,
	  int force, int width)
{
    TextLineList *buf = h_env->buf;
    FILE *f = h_env->f;
    Str line = obuf->line, pass = NULL;
    char *hidden_anchor = NULL, *hidden_img = NULL, *hidden_bold = NULL,
	*hidden_under = NULL, *hidden_italic = NULL, *hidden_strike = NULL,
	*hidden_ins = NULL, *hidden_input = NULL, *hidden = NULL;

#ifdef DEBUG
    if (w3m_debug) {
	FILE *df = fopen("zzzproc1", "a");
	fprintf(df, "flushline(%s,%d,%d,%d)\n", obuf->line->ptr, indent, force,
		width);
	if (buf) {
	    TextLineListItem *p;
	    for (p = buf->first; p; p = p->next) {
		fprintf(df, "buf=\"%s\"\n", p->ptr->line->ptr);
	    }
	}
	fclose(df);
    }
#endif

    if (!(obuf->flag & (RB_SPECIAL & ~RB_NOBR)) && Strlastchar(line) == ' ') {
	Strshrink(line, 1);
	obuf->pos--;
    }

    append_tags(obuf);

    if (obuf->anchor.url)
	hidden = hidden_anchor = has_hidden_link(obuf, HTML_A);
    if (obuf->img_alt) {
	if ((hidden_img = has_hidden_link(obuf, HTML_IMG_ALT)) != NULL) {
	    if (!hidden || hidden_img < hidden)
		hidden = hidden_img;
	}
    }
    if (obuf->input_alt.in) {
	if ((hidden_input = has_hidden_link(obuf, HTML_INPUT_ALT)) != NULL) {
	    if (!hidden || hidden_input < hidden)
		hidden = hidden_input;
	}
    }
    if (obuf->in_bold) {
	if ((hidden_bold = has_hidden_link(obuf, HTML_B)) != NULL) {
	    if (!hidden || hidden_bold < hidden)
		hidden = hidden_bold;
	}
    }
    if (obuf->in_italic) {
	if ((hidden_italic = has_hidden_link(obuf, HTML_I)) != NULL) {
	    if (!hidden || hidden_italic < hidden)
		hidden = hidden_italic;
	}
    }
    if (obuf->in_under) {
	if ((hidden_under = has_hidden_link(obuf, HTML_U)) != NULL) {
	    if (!hidden || hidden_under < hidden)
		hidden = hidden_under;
	}
    }
    if (obuf->in_strike) {
	if ((hidden_strike = has_hidden_link(obuf, HTML_S)) != NULL) {
	    if (!hidden || hidden_strike < hidden)
		hidden = hidden_strike;
	}
    }
    if (obuf->in_ins) {
	if ((hidden_ins = has_hidden_link(obuf, HTML_INS)) != NULL) {
	    if (!hidden || hidden_ins < hidden)
		hidden = hidden_ins;
	}
    }
    if (hidden) {
	pass = Strnew_charp(hidden);
	Strshrink(line, line->ptr + line->length - hidden);
    }

    if (!(obuf->flag & (RB_SPECIAL & ~RB_NOBR)) && obuf->pos > width) {
	char *tp = &line->ptr[obuf->bp.len - obuf->bp.tlen];
	char *ep = &line->ptr[line->length];

	if (obuf->bp.pos == obuf->pos && tp <= ep &&
	    tp > line->ptr && tp[-1] == ' ') {
	    bcopy(tp, tp - 1, ep - tp + 1);
	    line->length--;
	    obuf->pos--;
	}
    }

    if (obuf->anchor.url && !hidden_anchor)
	Strcat_charp(line, "</a>");
    if (obuf->img_alt && !hidden_img)
	Strcat_charp(line, "</img_alt>");
    if (obuf->input_alt.in && !hidden_input)
	Strcat_charp(line, "</input_alt>");
    if (obuf->in_bold && !hidden_bold)
	Strcat_charp(line, "</b>");
    if (obuf->in_italic && !hidden_italic)
	Strcat_charp(line, "</i>");
    if (obuf->in_under && !hidden_under)
	Strcat_charp(line, "</u>");
    if (obuf->in_strike && !hidden_strike)
	Strcat_charp(line, "</s>");
    if (obuf->in_ins && !hidden_ins)
	Strcat_charp(line, "</ins>");

    if (obuf->top_margin > 0) {
	int i;
	struct html_feed_environ h;
	struct readbuffer o;
	struct environment e[1];

	init_henv(&h, &o, e, 1, NULL, width, indent);
	o.line = Strnew_size(width + 20);
	o.pos = obuf->pos;
	o.flag = obuf->flag;
	o.top_margin = -1;
	o.bottom_margin = -1;
	Strcat_charp(o.line, "<pre_int>");
	for (i = 0; i < o.pos; i++)
	    Strcat_char(o.line, ' ');
	Strcat_charp(o.line, "</pre_int>");
	for (i = 0; i < obuf->top_margin; i++)
	    flushline(h_env, &o, indent, force, width);
    }

    if (force == 1 || obuf->flag & RB_NFLUSHED) {
	TextLine *lbuf = newTextLine(line, obuf->pos);
	if (RB_GET_ALIGN(obuf) == RB_CENTER) {
	    align(lbuf, width, ALIGN_CENTER);
	}
	else if (RB_GET_ALIGN(obuf) == RB_RIGHT) {
	    align(lbuf, width, ALIGN_RIGHT);
	}
	else if (RB_GET_ALIGN(obuf) == RB_LEFT && obuf->flag & RB_INTABLE) {
	    align(lbuf, width, ALIGN_LEFT);
	}
#ifdef FORMAT_NICE
	else if (obuf->flag & RB_FILL) {
	    char *p;
	    int rest, rrest;
	    int nspace, d, i;

	    rest = width - get_Str_strwidth(line);
	    if (rest > 1) {
		nspace = 0;
		for (p = line->ptr + indent; *p; p++) {
		    if (*p == ' ')
			nspace++;
		}
		if (nspace > 0) {
		    int indent_here = 0;
		    d = rest / nspace;
		    p = line->ptr;
		    while (IS_SPACE(*p)) {
			p++;
			indent_here++;
		    }
		    rrest = rest - d * nspace;
		    line = Strnew_size(width + 1);
		    for (i = 0; i < indent_here; i++)
			Strcat_char(line, ' ');
		    for (; *p; p++) {
			Strcat_char(line, *p);
			if (*p == ' ') {
			    for (i = 0; i < d; i++)
				Strcat_char(line, ' ');
			    if (rrest > 0) {
				Strcat_char(line, ' ');
				rrest--;
			    }
			}
		    }
		    lbuf = newTextLine(line, width);
		}
	    }
	}
#endif				/* FORMAT_NICE */
#ifdef TABLE_DEBUG
	if (w3m_debug) {
	    FILE *f = fopen("zzzproc1", "a");
	    fprintf(f, "pos=%d,%d, maxlimit=%d\n",
		    visible_length(lbuf->line->ptr), lbuf->pos,
		    h_env->maxlimit);
	    fclose(f);
	}
#endif
	if (lbuf->pos > h_env->maxlimit)
	    h_env->maxlimit = lbuf->pos;
	if (buf)
	    pushTextLine(buf, lbuf);
	else if (f) {
	    Strfputs(Str_conv_to_halfdump(lbuf->line), f);
	    fputc('\n', f);
	}
	if (obuf->flag & RB_SPECIAL || obuf->flag & RB_NFLUSHED)
	    h_env->blank_lines = 0;
	else
	    h_env->blank_lines++;
    }
    else {
	char *p = line->ptr, *q;
	Str tmp = Strnew(), tmp2 = Strnew();

#define APPEND(str) \
	if (buf) \
	    appendTextLine(buf,(str),0); \
	else if (f) \
	    Strfputs((str),f)

	while (*p) {
	    q = p;
	    if (sloppy_parse_line(&p)) {
		Strcat_charp_n(tmp, q, p - q);
		if (force == 2) {
		    APPEND(tmp);
		}
		else
		    Strcat(tmp2, tmp);
		Strclear(tmp);
	    }
	}
	if (force == 2) {
	    if (pass) {
		APPEND(pass);
	    }
	    pass = NULL;
	}
	else {
	    if (pass)
		Strcat(tmp2, pass);
	    pass = tmp2;
	}
    }

    if (obuf->bottom_margin > 0) {
	int i;
	struct html_feed_environ h;
	struct readbuffer o;
	struct environment e[1];

	init_henv(&h, &o, e, 1, NULL, width, indent);
	o.line = Strnew_size(width + 20);
	o.pos = obuf->pos;
	o.flag = obuf->flag;
	o.top_margin = -1;
	o.bottom_margin = -1;
	Strcat_charp(o.line, "<pre_int>");
	for (i = 0; i < o.pos; i++)
	    Strcat_char(o.line, ' ');
	Strcat_charp(o.line, "</pre_int>");
	for (i = 0; i < obuf->bottom_margin; i++)
	    flushline(h_env, &o, indent, force, width);
    }
    if (obuf->top_margin < 0 || obuf->bottom_margin < 0)
	return;

    obuf->line = Strnew_size(256);
    obuf->pos = 0;
    obuf->top_margin = 0;
    obuf->bottom_margin = 0;
    set_space_to_prevchar(obuf->prevchar);
    obuf->bp.init_flag = 1;
    obuf->flag &= ~RB_NFLUSHED;
    set_breakpoint(obuf, 0);
    obuf->prev_ctype = PC_ASCII;
    link_stack = NULL;
    fillline(obuf, indent);
    if (pass)
	passthrough(obuf, pass->ptr, 0);
    if (!hidden_anchor && obuf->anchor.url) {
	Str tmp;
	if (obuf->anchor.hseq > 0)
	    obuf->anchor.hseq = -obuf->anchor.hseq;
	tmp = Sprintf("<A HSEQ=\"%d\" HREF=\"", obuf->anchor.hseq);
	Strcat_charp(tmp, html_quote(obuf->anchor.url));
	if (obuf->anchor.target) {
	    Strcat_charp(tmp, "\" TARGET=\"");
	    Strcat_charp(tmp, html_quote(obuf->anchor.target));
	}
	if (obuf->anchor.referer) {
	    Strcat_charp(tmp, "\" REFERER=\"");
	    Strcat_charp(tmp, html_quote(obuf->anchor.referer));
	}
	if (obuf->anchor.title) {
	    Strcat_charp(tmp, "\" TITLE=\"");
	    Strcat_charp(tmp, html_quote(obuf->anchor.title));
	}
	if (obuf->anchor.accesskey) {
	    char *c = html_quote_char(obuf->anchor.accesskey);
	    Strcat_charp(tmp, "\" ACCESSKEY=\"");
	    if (c)
		Strcat_charp(tmp, c);
	    else
		Strcat_char(tmp, obuf->anchor.accesskey);
	}
	Strcat_charp(tmp, "\">");
	push_tag(obuf, tmp->ptr, HTML_A);
    }
    if (!hidden_img && obuf->img_alt) {
	Str tmp = Strnew_charp("<IMG_ALT SRC=\"");
	Strcat_charp(tmp, html_quote(obuf->img_alt->ptr));
	Strcat_charp(tmp, "\">");
	push_tag(obuf, tmp->ptr, HTML_IMG_ALT);
    }
    if (!hidden_input && obuf->input_alt.in) {
	Str tmp;
	if (obuf->input_alt.hseq > 0)
	    obuf->input_alt.hseq = - obuf->input_alt.hseq;
	tmp = Sprintf("<INPUT_ALT hseq=\"%d\" fid=\"%d\" name=\"%s\" type=\"%s\" value=\"%s\">",
		     obuf->input_alt.hseq,
		     obuf->input_alt.fid,
		     obuf->input_alt.name ? obuf->input_alt.name->ptr : "",
		     obuf->input_alt.type ? obuf->input_alt.type->ptr : "",
		     obuf->input_alt.value ? obuf->input_alt.value->ptr : "");
	push_tag(obuf, tmp->ptr, HTML_INPUT_ALT);
    }
    if (!hidden_bold && obuf->in_bold)
	push_tag(obuf, "<B>", HTML_B);
    if (!hidden_italic && obuf->in_italic)
	push_tag(obuf, "<I>", HTML_I);
    if (!hidden_under && obuf->in_under)
	push_tag(obuf, "<U>", HTML_U);
    if (!hidden_strike && obuf->in_strike)
	push_tag(obuf, "<S>", HTML_S);
    if (!hidden_ins && obuf->in_ins)
	push_tag(obuf, "<INS>", HTML_INS);
}