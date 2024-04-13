save_fonteffect(struct html_feed_environ *h_env, struct readbuffer *obuf)
{
    if (obuf->fontstat_sp < FONT_STACK_SIZE)
	bcopy(obuf->fontstat, obuf->fontstat_stack[obuf->fontstat_sp],
	      FONTSTAT_SIZE);
    obuf->fontstat_sp++;
    if (obuf->in_bold)
	push_tag(obuf, "</b>", HTML_N_B);
    if (obuf->in_italic)
	push_tag(obuf, "</i>", HTML_N_I);
    if (obuf->in_under)
	push_tag(obuf, "</u>", HTML_N_U);
    if (obuf->in_strike)
	push_tag(obuf, "</s>", HTML_N_S);
    if (obuf->in_ins)
	push_tag(obuf, "</ins>", HTML_N_INS);
    bzero(obuf->fontstat, FONTSTAT_SIZE);
}