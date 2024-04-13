only_one_window(void)
{
    int		count = 0;
    win_T	*wp;

    /* If there is another tab page there always is another window. */
    if (first_tabpage->tp_next != NULL)
	return FALSE;

    FOR_ALL_WINDOWS(wp)
	if (wp->w_buffer != NULL
		&& (!((bt_help(wp->w_buffer) && !bt_help(curbuf))
# ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
# endif
	     ) || wp == curwin) && wp != aucmd_win)
	    ++count;
    return (count <= 1);
}