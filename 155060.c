win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    if (win == NULL)
	return FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    if (wp == win)
		return TRUE;
	}
#ifdef FEAT_TEXT_PROP
	for (wp = tp->tp_first_popupwin; wp != NULL; wp = wp->w_next)
	    if (wp == win)
		return TRUE;
#endif
    }
    return win_valid_popup(win);
}