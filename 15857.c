findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_line;
    int		len;

    conv_line = string_convert(&st->vimconv, st->lbuf, NULL);
    if (conv_line == NULL)
	return;

    // Copy or swap lbuf and conv_line.
    len = (int)STRLEN(conv_line) + 1;
    if (len > st->lbuf_size)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_line;
	st->lbuf_size = len;
    }
    else
    {
	STRCPY(st->lbuf, conv_line);
	vim_free(conv_line);
    }
}