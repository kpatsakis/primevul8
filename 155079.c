shell_new_rows(void)
{
    int		h = (int)ROWS_AVAIL;

    if (firstwin == NULL)	/* not initialized yet */
	return;
    if (h < frame_minheight(topframe, NULL))
	h = frame_minheight(topframe, NULL);

    /* First try setting the heights of windows with 'winfixheight'.  If
     * that doesn't result in the right height, forget about that option. */
    frame_new_height(topframe, h, FALSE, TRUE);
    if (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE);

    (void)win_comp_pos();		/* recompute w_winrow and w_wincol */
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

#if 0
    /* Disabled: don't want making the screen smaller make a window larger. */
    if (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}