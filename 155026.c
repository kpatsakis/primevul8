win_close_othertab(win_T *win, int free_buf, tabpage_T *tp)
{
    win_T	*wp;
    int		dir;
    tabpage_T   *ptp = NULL;
    int		free_tp = FALSE;

    /* Get here with win->w_buffer == NULL when win_close() detects the tab
     * page changed. */
    if (win->w_closing || (win->w_buffer != NULL
					       && win->w_buffer->b_locked > 0))
	return; /* window is already being closed */

    if (win->w_buffer != NULL)
	/* Close the link to the buffer. */
	close_buffer(win, win->w_buffer, free_buf ? DOBUF_UNLOAD : 0, FALSE);

    /* Careful: Autocommands may have closed the tab page or made it the
     * current tab page.  */
    for (ptp = first_tabpage; ptp != NULL && ptp != tp; ptp = ptp->tp_next)
	;
    if (ptp == NULL || tp == curtab)
	return;

    /* Autocommands may have closed the window already. */
    for (wp = tp->tp_firstwin; wp != NULL && wp != win; wp = wp->w_next)
	;
    if (wp == NULL)
	return;

    /* When closing the last window in a tab page remove the tab page. */
    if (tp->tp_firstwin == tp->tp_lastwin)
    {
	if (tp == first_tabpage)
	    first_tabpage = tp->tp_next;
	else
	{
	    for (ptp = first_tabpage; ptp != NULL && ptp->tp_next != tp;
							   ptp = ptp->tp_next)
		;
	    if (ptp == NULL)
	    {
		internal_error("win_close_othertab()");
		return;
	    }
	    ptp->tp_next = tp->tp_next;
	}
	free_tp = TRUE;
    }

    /* Free the memory used for the window. */
    win_free_mem(win, &dir, tp);

    if (free_tp)
	free_tabpage(tp);
}