win_move_after(win_T *win1, win_T *win2)
{
    int		height;

    /* check if the arguments are reasonable */
    if (win1 == win2)
	return;

    /* check if there is something to do */
    if (win2->w_next != win1)
    {
	if (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("INTERNAL: trying to move a window into another frame");
	    return;
	}

	/* may need move the status line/vertical separator of the last window
	 * */
	if (win1 == lastwin)
	{
	    height = win1->w_prev->w_status_height;
	    win1->w_prev->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    if (win1->w_prev->w_vsep_width == 1)
	    {
		/* Remove the vertical separator from the last-but-one window,
		 * add it to the last window.  Adjust the frame widths. */
		win1->w_prev->w_vsep_width = 0;
		win1->w_prev->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	else if (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    if (win1->w_vsep_width == 1)
	    {
		/* Remove the vertical separator from win1, add it to the last
		 * window, win2.  Adjust the frame widths. */
		win2->w_vsep_width = 1;
		win2->w_frame->fr_width += 1;
		win1->w_vsep_width = 0;
		win1->w_frame->fr_width -= 1;
	    }
	}
	win_remove(win1, NULL);
	frame_remove(win1->w_frame);
	win_append(win2, win1);
	frame_append(win2->w_frame, win1->w_frame);

	(void)win_comp_pos();	/* recompute w_winrow for all windows */
	redraw_later(NOT_VALID);
    }
    win_enter(win1, FALSE);
}