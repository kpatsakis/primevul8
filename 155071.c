frame_setheight(frame_T *curfrp, int height)
{
    int		room;		/* total number of lines available */
    int		take;		/* number of lines taken from other windows */
    int		room_cmdline;	/* lines available from cmdline */
    int		run;
    frame_T	*frp;
    int		h;
    int		room_reserved;

    /* If the height already is the desired value, nothing to do. */
    if (curfrp->fr_height == height)
	return;

    if (curfrp->fr_parent == NULL)
    {
	/* topframe: can only change the command line */
	if (height > ROWS_AVAIL)
	    height = ROWS_AVAIL;
	if (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE);
    }
    else if (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	/* Row of frames: Also need to resize frames left and right of this
	 * one.  First check for the minimal height of these. */
	h = frame_minheight(curfrp->fr_parent, NULL);
	if (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    else
    {
	/*
	 * Column of frames: try to change only frames in this column.
	 */
	/*
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the height to it.
	 * Try not to reduce the height of a window with 'winfixheight' set.
	 */
	for (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		if (frp != curfrp
			&& frp->fr_win != NULL
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		if (frp != curfrp)
		    room -= frame_minheight(frp, NULL);
	    }
	    if (curfrp->fr_width != Columns)
		room_cmdline = 0;
	    else
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		if (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    if (height <= room + room_cmdline)
		break;
	    if (run == 2 || curfrp->fr_width == Columns)
	    {
		if (height > room + room_cmdline)
		    height = room + room_cmdline;
		break;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (int)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	/* If there is not enough room, also reduce the height of a window
	 * with 'winfixheight' set. */
	if (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	/* If there is only a 'winfixheight' window and making the
	 * window smaller, need to make the other window taller. */
	if (take < 0 && room - curfrp->fr_height < room_reserved)
	    room_reserved = 0;

	if (take > 0 && room_cmdline > 0)
	{
	    /* use lines from cmdline first */
	    if (take < room_cmdline)
		room_cmdline = take;
	    take -= room_cmdline;
	    topframe->fr_height += room_cmdline;
	}

	/*
	 * set the current frame to the new height
	 */
	frame_new_height(curfrp, height, FALSE, FALSE);

	/*
	 * First take lines from the frames after the current frame.  If
	 * that is not enough, takes lines from frames above the current
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
		h = frame_minheight(frp, NULL);
		if (room_reserved > 0
			&& frp->fr_win != NULL
			&& frp->fr_win->w_p_wfh)
		{
		    if (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    else
		    {
			if (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		else
		{
		    if (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE);
		    }
		    else
		    {
			frame_new_height(frp, frp->fr_height - take,
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