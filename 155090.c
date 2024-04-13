buf_jump_open_tab(buf_T *buf)
{
    win_T	*wp = buf_jump_open_win(buf);
    tabpage_T	*tp;

    if (wp != NULL)
	return wp;

    FOR_ALL_TABPAGES(tp)
	if (tp != curtab)
	{
	    for (wp = tp->tp_firstwin; wp != NULL; wp = wp->w_next)
		if (wp->w_buffer == buf)
		    break;
	    if (wp != NULL)
	    {
		goto_tabpage_win(tp, wp);
		if (curwin != wp)
		    wp = NULL;	/* something went wrong */
		break;
	    }
	}
    return wp;
}