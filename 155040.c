frame_minwidth(
    frame_T	*topfrp,
    win_T	*next_curwin)	/* use p_wh and p_wiw for next_curwin */
{
    frame_T	*frp;
    int		m, n;

    if (topfrp->fr_win != NULL)
    {
	if (topfrp->fr_win == next_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	else
	{
	    /* window: minimal width of the window plus separator column */
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    /* Current window is minimal one column wide */
	    if (p_wmw == 0 && topfrp->fr_win == curwin && next_curwin == NULL)
		++m;
	}
    }
    else if (topfrp->fr_layout == FR_COL)
    {
	/* get the minimal width from each frame in this column */
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, next_curwin);
	    if (n > m)
		m = n;
	}
    }
    else
    {
	/* Add up the minimal widths for all frames in this row. */
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, next_curwin);
    }

    return m;
}