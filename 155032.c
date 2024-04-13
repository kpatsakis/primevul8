close_windows(
    buf_T	*buf,
    int		keep_curwin)	    /* don't close "curwin" */
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    int		h = tabline_height();
    int		count = tabpage_index(NULL);

    ++RedrawingDisabled;

    for (wp = firstwin; wp != NULL && !ONE_WINDOW; )
    {
	if (wp->w_buffer == buf && (!keep_curwin || wp != curwin)
		&& !(wp->w_closing || wp->w_buffer->b_locked > 0))
	{
	    if (win_close(wp, FALSE) == FAIL)
		/* If closing the window fails give up, to avoid looping
		 * forever. */
		break;

	    /* Start all over, autocommands may change the window layout. */
	    wp = firstwin;
	}
	else
	    wp = wp->w_next;
    }

    /* Also check windows in other tab pages. */
    for (tp = first_tabpage; tp != NULL; tp = nexttp)
    {
	nexttp = tp->tp_next;
	if (tp != curtab)
	    for (wp = tp->tp_firstwin; wp != NULL; wp = wp->w_next)
		if (wp->w_buffer == buf
		    && !(wp->w_closing || wp->w_buffer->b_locked > 0))
		{
		    win_close_othertab(wp, FALSE, tp);

		    /* Start all over, the tab page may be closed and
		     * autocommands may change the window layout. */
		    nexttp = first_tabpage;
		    break;
		}
    }

    --RedrawingDisabled;

    if (count != tabpage_index(NULL))
	apply_autocmds(EVENT_TABCLOSED, NULL, NULL, FALSE, curbuf);

    redraw_tabline = TRUE;
    if (h != tabline_height())
	shell_new_rows();
}