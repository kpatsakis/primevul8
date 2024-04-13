valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	if (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		if (win_valid_any_tab(wp))
		    return TRUE;
	    }
	    return FALSE;
	}
    }
    /* shouldn't happen */
    return FALSE;
}