push_nchars(struct readbuffer *obuf, int width,
	    char *str, int len, Lineprop mode)
{
    append_tags(obuf);
    Strcat_charp_n(obuf->line, str, len);
    obuf->pos += width;
    if (width > 0) {
	set_prevchar(obuf->prevchar, str, len);
	obuf->prev_ctype = mode;
    }
    obuf->flag |= RB_NFLUSHED;
}