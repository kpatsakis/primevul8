frame_setwidth(frame_T *curfrp, int width)
{
    int		room;		/* total number of lines available */
    int		take;		/* number of lines taken from other windows */
    int		run;
    frame_T	*frp;
    int		w;
    int		room_reserved;

    /* If the width already is the desired value, nothing to do. */
    if (curfrp->fr_width == width)
	return;

    if (curfrp->fr_parent == NULL)
	/* topframe: can't change width */
	return;

    if (curfrp->fr_parent->fr_layout == FR_COL)
    {
	/* Column of frames: Also need to resize frames above and below of
	 * this one.  First check for the minimal width of these. */
	w = frame_minwidth(curfrp->fr_parent, NULL);
	if (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    else
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	for (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		if (frp != curfrp
			&& frp->fr_win != NULL
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		if (frp != curfrp)
		    room -= frame_minwidth(frp, NULL);
	    }

	    if (width <= room)
		break;
	    if (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		if (width > room)
		    width = room;
		break;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (int)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	/* If there is not enough room, also reduce the width of a window
	 * with 'winfixwidth' set. */
	if (width > room - room_reserved)
	    room_reserved = room - width;
	/* If there is only a 'winfixwidth' window and making the
	 * window smaller, need to make the other window narrower. */
	if (take < 0 && room - curfrp->fr_width < room_reserved)
	    room_reserved = 0;

	/*
	 * set the current frame to the new width
	 */
	frame_new_width(curfrp, width, FALSE, FALSE);

	/*
	 * First take lines from the frames right of the current frame.  If
	 * that is not enough, takes lines from frames left of the current
	 * frame.
	 */
	for (run = 0; run < 2; ++run)
	{
	    if (run == 0)
		frp = curfrp->fr_next;	/* 1st run: start with next window */
	    else
		frp = curfrp->fr_prev;	/* 2nd run: start with prev window */
	    while (frp != NULL && take != 0)
	    {
		w = frame_minwidth(frp, NULL);
		if (room_reserved > 0
			&& frp->fr_win != NULL
			&& frp->fr_win->w_p_wfw)
		{
		    if (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    else
		    {
			if (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		else
		{
		    if (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    else
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		if (run == 0)
		    frp = frp->fr_next;
		else
		    frp = frp->fr_prev;
	    }
	}
    }
}