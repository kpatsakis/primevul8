frame_new_width(
    frame_T	*topfrp,
    int		width,
    int		leftfirst,	/* resize leftmost contained frame first */
    int		wfw)		/* obey 'winfixwidth' when there is a choice;
				   may cause the width not to be set */
{
    frame_T	*frp;
    int		extra_cols;
    int		w;
    win_T	*wp;

    if (topfrp->fr_layout == FR_LEAF)
    {
	/* Simple case: just one window. */
	wp = topfrp->fr_win;
	/* Find out if there are any windows right of this one. */
	for (frp = topfrp; frp->fr_parent != NULL; frp = frp->fr_parent)
	    if (frp->fr_parent->fr_layout == FR_ROW && frp->fr_next != NULL)
		break;
	if (frp->fr_parent == NULL)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    else if (topfrp->fr_layout == FR_COL)
    {
	do
	{
	    /* All frames in this column get the same new width. */
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		if (frp->fr_width > width)
		{
		    /* Could not fit the windows, make whole column wider. */
		    width = frp->fr_width;
		    break;
		}
	    }
	} while (frp != NULL);
    }
    else    /* fr_layout == FR_ROW */
    {
	/* Complicated case: Resize a row of frames.  Resize the rightmost
	 * frame first, frames left of it when needed. */

	frp = topfrp->fr_child;
	if (wfw)
	    /* Advance past frames with one window with 'wfw' set. */
	    while (frame_fixed_width(frp))
	    {
		frp = frp->fr_next;
		if (frp == NULL)
		    return;	    /* no frame without 'wfw', give up */
	    }
	if (!leftfirst)
	{
	    /* Find the rightmost frame of this row */
	    while (frp->fr_next != NULL)
		frp = frp->fr_next;
	    if (wfw)
		/* Advance back for frames with one window with 'wfw' set. */
		while (frame_fixed_width(frp))
		    frp = frp->fr_prev;
	}

	extra_cols = width - topfrp->fr_width;
	if (extra_cols < 0)
	{
	    /* reduce frame width, rightmost frame first */
	    while (frp != NULL)
	    {
		w = frame_minwidth(frp, NULL);
		if (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		else
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    break;
		}
		if (leftfirst)
		{
		    do
			frp = frp->fr_next;
		    while (wfw && frp != NULL && frame_fixed_width(frp));
		}
		else
		{
		    do
			frp = frp->fr_prev;
		    while (wfw && frp != NULL && frame_fixed_width(frp));
		}
		/* Increase "width" if we could not reduce enough frames. */
		if (frp == NULL)
		    width -= extra_cols;
	    }
	}
	else if (extra_cols > 0)
	{
	    /* increase width of rightmost frame */
	    frame_new_width(frp, frp->fr_width + extra_cols, leftfirst, wfw);
	}
    }
    topfrp->fr_width = width;
}