push_char(struct readbuffer *obuf, int pre_mode, char ch)
{
    check_breakpoint(obuf, pre_mode, &ch);
    Strcat_char(obuf->line, ch);
    obuf->pos++;
    set_prevchar(obuf->prevchar, &ch, 1);
    if (ch != ' ')
	obuf->prev_ctype = PC_ASCII;
    obuf->flag |= RB_NFLUSHED;
}