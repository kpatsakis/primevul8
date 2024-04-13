frame_new_height(
    frame_T	*topfrp,
    int		height,
    int		topfirst,	/* resize topmost contained frame first */
    int		wfh)		/* obey 'winfixheight' when there is a choice;
				   may cause the height not to be set */
{
    frame_T	*frp;
    int		extra_lines;
    int		h;

    if (topfrp->fr_win != NULL)
    {
	/* Simple case: just one window. */
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    else if (topfrp->fr_layout == FR_ROW)
    {
	do
	{
	    /* All frames in this row get the same new height. */
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh);
		if (frp->fr_height > height)
		{
		    /* Could not fit the windows, make the whole row higher. */
		    height = frp->fr_height;
		    break;
		}
	    }
	}
	while (frp != NULL);
    }
    else    /* fr_layout == FR_COL */
    {
	/* Complicated case: Resize a column of frames.  Resize the bottom
	 * frame first, frames above that when needed. */

	frp = topfrp->fr_child;
	if (wfh)
	    /* Advance past frames with one window with 'wfh' set. */
	    while (frame_fixed_height(frp))
	    {
		frp = frp->fr_next;
		if (frp == NULL)
		    return;	    /* no frame without 'wfh', give up */
	    }
	if (!topfirst)
	{
	    /* Find the bottom frame of this column */
	    while (frp->fr_next != NULL)
		frp = frp->fr_next;
	    if (wfh)
		/* Advance back for frames with one window with 'wfh' set. */
		while (frame_fixed_height(frp))
		    frp = frp->fr_prev;
	}

	extra_lines = height - topfrp->fr_height;
	if (extra_lines < 0)
	{
	    /* reduce height of contained frames, bottom or top frame first */
	    while (frp != NULL)
	    {
		h = frame_minheight(frp, NULL);
		if (frp->fr_height + extra_lines < h)
		{
		    extra_lines += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh);
		}
		else
		{
		    frame_new_height(frp, frp->fr_height + extra_lines,
							       topfirst, wfh);
		    break;
		}
		if (topfirst)
		{
		    do
			frp = frp->fr_next;
		    while (wfh && frp != NULL && frame_fixed_height(frp));
		}
		else
		{
		    do
			frp = frp->fr_prev;
		    while (wfh && frp != NULL && frame_fixed_height(frp));
		}
		/* Increase "height" if we could not reduce enough frames. */
		if (frp == NULL)
		    height -= extra_lines;
	    }
	}
	else if (extra_lines > 0)
	{
	    /* increase height of bottom or top frame */
	    frame_new_height(frp, frp->fr_height + extra_lines, topfirst, wfh);
	}
    }
    topfrp->fr_height = height;
}