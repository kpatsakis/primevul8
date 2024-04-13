win_drag_vsep_line(win_T *dragwin, int offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    int		room;
    int		left;	/* if TRUE, drag separator line left, otherwise right */
    int		n;

    fr = dragwin->w_frame;
    if (fr == topframe)		/* only one window (cannot happen?) */
	return;
    curfr = fr;
    fr = fr->fr_parent;
    /* When the parent frame is not a row of frames, its parent should be. */
    if (fr->fr_layout != FR_ROW)
    {
	if (fr == topframe)	/* only a column of windows (cannot happen?) */
	    return;
	curfr = fr;
	fr = fr->fr_parent;
    }

    /* If this is the last frame in a row, may want to resize a parent
     * frame instead. */
    while (curfr->fr_next == NULL)
    {
	if (fr == topframe)
	    break;
	curfr = fr;
	fr = fr->fr_parent;
	if (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    if (offset < 0) /* drag left */
    {
	left = TRUE;
	offset = -offset;
	/* sum up the room of the current frame and left of it */
	room = 0;
	for (fr = fr->fr_child; ; fr = fr->fr_next)
	{
	    room += fr->fr_width - frame_minwidth(fr, NULL);
	    if (fr == curfr)
		break;
	}
	fr = curfr->fr_next;		/* put fr at frame that grows */
    }
    else    /* drag right */
    {
	left = FALSE;
	/* sum up the room of frames right of the current one */
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_width - frame_minwidth(fr, NULL);
	fr = curfr;			/* put fr at window that grows */
    }

    if (room < offset)		/* Not enough room */
	offset = room;		/* Move as far as we can */
    if (offset <= 0)		/* No room at all, quit. */
	return;
    if (fr == NULL)
	return;			/* Safety check, should not happen. */

    /* grow frame fr by offset lines */
    frame_new_width(fr, fr->fr_width + offset, left, FALSE);

    /* shrink other frames: current and at the left or at the right */
    if (left)
	fr = curfr;		/* current frame gets smaller */
    else
	fr = curfr->fr_next;	/* next frame gets smaller */

    while (fr != NULL && offset > 0)
    {
	n = frame_minwidth(fr, NULL);
	if (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !left, FALSE);
	}
	else
	{
	    frame_new_width(fr, fr->fr_width - offset, !left, FALSE);
	    break;
	}
	if (left)
	    fr = fr->fr_prev;
	else
	    fr = fr->fr_next;
    }
    (void)win_comp_pos();
    redraw_all_later(NOT_VALID);
}