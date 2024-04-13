do_blankline(struct html_feed_environ *h_env, struct readbuffer *obuf,
	     int indent, int indent_incr, int width)
{
    if (h_env->blank_lines == 0)
	flushline(h_env, obuf, indent, 1, width);
}