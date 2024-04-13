frame_minheight(frame_T *topfrp, win_T *next_curwin)
{
    frame_T	*frp;
    int		m;
    int		n;

    if (topfrp->fr_win != NULL)
    {
	if (topfrp->fr_win == next_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	else
	{
	    /* window: minimal height of the window plus status line */
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    if (topfrp->fr_win == curwin && next_curwin == NULL)
	    {
		/* Current window is minimal one line high and WinBar is
		 * visible. */
		if (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    else if (topfrp->fr_layout == FR_ROW)
    {
	/* get the minimal height from each frame in this row */
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, next_curwin);
	    if (n > m)
		m = n;
	}
    }
    else
    {
	/* Add up the minimal heights for all frames in this column. */
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, next_curwin);
    }

    return m;
}