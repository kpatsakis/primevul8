close_last_window_tabpage(
    win_T	*win,
    int		free_buf,
    tabpage_T   *prev_curtab)
{
    if (ONE_WINDOW)
    {
	buf_T	*old_curbuf = curbuf;

	/*
	 * Closing the last window in a tab page.  First go to another tab
	 * page and then close the window and the tab page.  This avoids that
	 * curwin and curtab are invalid while we are freeing memory, they may
	 * be used in GUI events.
	 * Don't trigger autocommands yet, they may use wrong values, so do
	 * that below.
	 */
	goto_tabpage_tp(alt_tabpage(), FALSE, TRUE);
	redraw_tabline = TRUE;

	/* Safety check: Autocommands may have closed the window when jumping
	 * to the other tab page. */
	if (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	{
	    int	    h = tabline_height();

	    win_close_othertab(win, free_buf, prev_curtab);
	    if (h != tabline_height())
		shell_new_rows();
	}
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
	/* Since goto_tabpage_tp above did not trigger *Enter autocommands, do
	 * that now. */
	apply_autocmds(EVENT_TABCLOSED, NULL, NULL, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, NULL, NULL, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, NULL, NULL, FALSE, curbuf);
	if (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
	return TRUE;
    }
    return FALSE;
}