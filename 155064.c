shell_new_columns(void)
{
    if (firstwin == NULL)	/* not initialized yet */
	return;

    /* First try setting the widths of windows with 'winfixwidth'.  If that
     * doesn't result in the right width, forget about that option. */
    frame_new_width(topframe, (int)Columns, FALSE, TRUE);
    if (!frame_check_width(topframe, Columns))
	frame_new_width(topframe, (int)Columns, FALSE, FALSE);

    (void)win_comp_pos();		/* recompute w_winrow and w_wincol */
#if 0
    /* Disabled: don't want making the screen smaller make a window larger. */
    if (p_ea)
	win_equal(curwin, FALSE, 'h');
#endif
}