proc_mchar(struct readbuffer *obuf, int pre_mode,
	   int width, char **str, Lineprop mode)
{
    check_breakpoint(obuf, pre_mode, *str);
    obuf->pos += width;
    Strcat_charp_n(obuf->line, *str, get_mclen(*str));
    if (width > 0) {
	set_prevchar(obuf->prevchar, *str, 1);
	if (**str != ' ')
	    obuf->prev_ctype = mode;
    }
    (*str) += get_mclen(*str);
    obuf->flag |= RB_NFLUSHED;
}