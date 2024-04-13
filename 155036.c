win_setwidth_win(int width, win_T *wp)
{
    /* Always keep current window at least one column wide, even when
     * 'winminwidth' is zero. */
    if (wp == curwin)
    {
	if (width < p_wmw)
	    width = p_wmw;
	if (width == 0)
	    width = 1;
    }

    frame_setwidth(wp->w_frame, width + wp->w_vsep_width);

    /* recompute the window positions */
    (void)win_comp_pos();

    redraw_all_later(NOT_VALID);
}