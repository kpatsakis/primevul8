win_drag_status_line(win_T *dragwin, int offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    int		room;
    int		row;
    int		up;	/* if TRUE, drag status line up, otherwise down */
    int		n;

    fr = dragwin->w_frame;
    curfr = fr;
    if (fr != topframe)		/* more than one window */
    {
	fr = fr->fr_parent;
	/* When the parent frame is not a column of frames, its parent should
	 * be. */
	if (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    if (fr != topframe)	/* only a row of windows, may drag statusline */
		fr = fr->fr_parent;
	}
    }

    /* If this is the last frame in a column, may want to resize the parent
     * frame instead (go two up to skip a row of frames). */
    while (curfr != topframe && curfr->fr_next == NULL)
    {
	if (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	if (fr != topframe)
	    fr = fr->fr_parent;
    }

    if (offset < 0) /* drag up */
    {
	up = TRUE;
	offset = -offset;
	/* sum up the room of the current frame and above it */
	if (fr == curfr)
	{
	    /* only one window */
	    room = fr->fr_height - frame_minheight(fr, NULL);
	}
	else
	{
	    room = 0;
	    for (fr = fr->fr_child; ; fr = fr->fr_next)
	    {
		room += fr->fr_height - frame_minheight(fr, NULL);
		if (fr == curfr)
		    break;
	    }
	}
	fr = curfr->fr_next;		/* put fr at frame that grows */
    }
    else    /* drag down */
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	if (curfr->fr_next == NULL)
	    room -= 1;
	else
	    room -= p_ch;
	if (room < 0)
	    room = 0;
	/* sum up the room of frames below of the current one */
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_height - frame_minheight(fr, NULL);
	fr = curfr;			/* put fr at window that grows */
    }

    if (room < offset)		/* Not enough room */
	offset = room;		/* Move as far as we can */
    if (offset <= 0)
	return;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    if (fr != NULL)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE);

    if (up)
	fr = curfr;		/* current frame gets smaller */
    else
	fr = curfr->fr_next;	/* next frame gets smaller */

    /*
     * Now make the other frames smaller.
     */
    while (fr != NULL && offset > 0)
    {
	n = frame_minheight(fr, NULL);
	if (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE);
	}
	else
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE);
	    break;
	}
	if (up)
	    fr = fr->fr_prev;
	else
	    fr = fr->fr_next;
    }
    row = win_comp_pos();
    screen_fill(row, cmdline_row, 0, (int)Columns, ' ', ' ', 0);
    cmdline_row = row;
    p_ch = Rows - cmdline_row;
    if (p_ch < 1)
	p_ch = 1;
    curtab->tp_ch_used = p_ch;
    redraw_all_later(SOME_VALID);
    showmode();
}