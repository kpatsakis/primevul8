push_spaces(struct readbuffer *obuf, int pre_mode, int width)
{
    int i;

    if (width <= 0)
	return;
    check_breakpoint(obuf, pre_mode, " ");
    for (i = 0; i < width; i++)
	Strcat_char(obuf->line, ' ');
    obuf->pos += width;
    set_space_to_prevchar(obuf->prevchar);
    obuf->flag |= RB_NFLUSHED;
}