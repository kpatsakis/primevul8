purgeline(struct html_feed_environ *h_env)
{
    char *p, *q;
    Str tmp;

    if (h_env->buf == NULL || h_env->blank_lines == 0)
	return;

    p = rpopTextLine(h_env->buf)->line->ptr;
    tmp = Strnew();
    while (*p) {
	q = p;
	if (sloppy_parse_line(&p)) {
	    Strcat_charp_n(tmp, q, p - q);
	}
    }
    appendTextLine(h_env->buf, tmp, 0);
    h_env->blank_lines--;
}