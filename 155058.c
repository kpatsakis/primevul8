command_height(void)
{
    int		h;
    frame_T	*frp;
    int		old_p_ch = curtab->tp_ch_used;

    /* Use the value of p_ch that we remembered.  This is needed for when the
     * GUI starts up, we can't be sure in what order things happen.  And when
     * p_ch was changed in another tab page. */
    curtab->tp_ch_used = p_ch;

    /* Find bottom frame with width of screen. */
    frp = lastwin->w_frame;
    while (frp->fr_width != Columns && frp->fr_parent != NULL)
	frp = frp->fr_parent;

    /* Avoid changing the height of a window with 'winfixheight' set. */
    while (frp->fr_prev != NULL && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    if (starting != NO_SCREEN)
    {
	cmdline_row = Rows - p_ch;

	if (p_ch > old_p_ch)		    /* p_ch got bigger */
	{
	    while (p_ch > old_p_ch)
	    {
		if (frp == NULL)
		{
		    emsg(_(e_noroom));
		    p_ch = old_p_ch;
		    curtab->tp_ch_used = p_ch;
		    cmdline_row = Rows - p_ch;
		    break;
		}
		h = frp->fr_height - frame_minheight(frp, NULL);
		if (h > p_ch - old_p_ch)
		    h = p_ch - old_p_ch;
		old_p_ch += h;
		frame_add_height(frp, -h);
		frp = frp->fr_prev;
	    }

	    /* Recompute window positions. */
	    (void)win_comp_pos();

	    /* clear the lines added to cmdline */
	    if (full_screen)
		screen_fill((int)(cmdline_row), (int)Rows, 0,
						   (int)Columns, ' ', ' ', 0);
	    msg_row = cmdline_row;
	    redraw_cmdline = TRUE;
	    return;
	}

	if (msg_row < cmdline_row)
	    msg_row = cmdline_row;
	redraw_cmdline = TRUE;
    }
    frame_add_height(frp, (int)(old_p_ch - p_ch));

    /* Recompute window positions. */
    if (frp != lastwin->w_frame)
	(void)win_comp_pos();
}