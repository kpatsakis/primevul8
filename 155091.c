win_size_restore(garray_T *gap)
{
    win_T	*wp;
    int		i, j;

    if (win_count() * 2 == gap->ga_len)
    {
	/* The order matters, because frames contain other frames, but it's
	 * difficult to get right. The easy way out is to do it twice. */
	for (j = 0; j < 2; ++j)
	{
	    i = 0;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((int *)gap->ga_data)[i++]);
		win_setheight_win(((int *)gap->ga_data)[i++], wp);
	    }
	}
	/* recompute the window positions */
	(void)win_comp_pos();
    }
}