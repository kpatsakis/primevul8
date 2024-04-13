check_breakpoint(struct readbuffer *obuf, int pre_mode, char *ch)
{
    int tlen, len = obuf->line->length;

    append_tags(obuf);
    if (pre_mode)
	return;
    tlen = obuf->line->length - len;
    if (tlen > 0
	|| is_boundary((unsigned char *)obuf->prevchar->ptr,
		       (unsigned char *)ch))
	set_breakpoint(obuf, tlen);
}