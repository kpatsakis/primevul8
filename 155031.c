win_setheight_win(int height, win_T *win)
{
    int		row;

    if (win == curwin)
    {
	/* Always keep current window at least one line high, even when
	 * 'winminheight' is zero. */
	if (height < p_wmh)
	    height = p_wmh;
	if (height == 0)
	    height = 1;
	height += WINBAR_HEIGHT(curwin);
    }

    frame_setheight(win->w_frame, height + win->w_status_height);

    /* recompute the window positions */
    row = win_comp_pos();

    /*
     * If there is extra space created between the last window and the command
     * line, clear it.
     */
    if (full_screen && msg_scrolled == 0 && row < cmdline_row)
	screen_fill(row, cmdline_row, 0, (int)Columns, ' ', ' ', 0);
    cmdline_row = row;
    msg_row = row;
    msg_col = 0;

    redraw_all_later(NOT_VALID);
}