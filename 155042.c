win_close(win_T *win, int free_buf)
{
    win_T	*wp;
    int		other_buffer = FALSE;
    int		close_curwin = FALSE;
    int		dir;
    int		help_window = FALSE;
    tabpage_T   *prev_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;

    if (ERROR_IF_POPUP_WINDOW)
	return FAIL;

    if (last_window())
    {
	emsg(_("E444: Cannot close last window"));
	return FAIL;
    }

    if (win->w_closing || (win->w_buffer != NULL
					       && win->w_buffer->b_locked > 0))
	return FAIL; /* window is already being closed */
    if (win_unlisted(win))
    {
	emsg(_("E813: Cannot close autocmd or popup window"));
	return FAIL;
    }
    if ((firstwin == aucmd_win || lastwin == aucmd_win) && one_window())
    {
	emsg(_("E814: Cannot close window, only autocmd window would remain"));
	return FAIL;
    }

    /* When closing the last window in a tab page first go to another tab page
     * and then close the window and the tab page to avoid that curwin and
     * curtab are invalid while we are freeing memory. */
    if (close_last_window_tabpage(win, free_buf, prev_curtab))
      return FAIL;

    /* When closing the help window, try restoring a snapshot after closing
     * the window.  Otherwise clear the snapshot, it's now invalid. */
    if (bt_help(win->w_buffer))
	help_window = TRUE;
    else
	clear_snapshot(curtab, SNAP_HELP_IDX);

    if (win == curwin)
    {
#ifdef FEAT_JOB_CHANNEL
	leaving_window(curwin);
#endif
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, NULL));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	if (wp->w_buffer != curbuf)
	{
	    other_buffer = TRUE;
	    win->w_closing = TRUE;
	    apply_autocmds(EVENT_BUFLEAVE, NULL, NULL, FALSE, curbuf);
	    if (!win_valid(win))
		return FAIL;
	    win->w_closing = FALSE;
	    if (last_window())
		return FAIL;
	}
	win->w_closing = TRUE;
	apply_autocmds(EVENT_WINLEAVE, NULL, NULL, FALSE, curbuf);
	if (!win_valid(win))
	    return FAIL;
	win->w_closing = FALSE;
	if (last_window())
	    return FAIL;
#ifdef FEAT_EVAL
	/* autocmds may abort script processing */
	if (aborting())
	    return FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    if (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_TEXT_PROP
    if (popup_win_closed(win) && !win_valid(win))
	return FAIL;
#endif
    win_close_buffer(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    if (only_one_window() && win_valid(win) && win->w_buffer == NULL
	    && (last_window() || curtab != prev_curtab
		|| close_last_window_tabpage(win, free_buf, prev_curtab)))
    {
	/* Autocommands have closed all windows, quit now.  Restore
	 * curwin->w_buffer, otherwise writing viminfo may fail. */
	if (curwin->w_buffer == NULL)
	    curwin->w_buffer = curbuf;
	getout(0);
    }

    /* Autocommands may have moved to another tab page. */
    if (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_buffer == NULL)
    {
	/* Need to close the window anyway, since the buffer is NULL. */
	win_close_othertab(win, FALSE, prev_curtab);
	return FAIL;
    }

    /* Autocommands may have closed the window already or closed the only
     * other window. */
    if (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, prev_curtab))
	return FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    ++split_disallowed;

    /* Free the memory used for the window and get the window that received
     * the screen space. */
    wp = win_free_mem(win, &dir, NULL);

    /* Make sure curwin isn't invalid.  It can cause severe trouble when
     * printing an error message.  For win_equal() curbuf needs to be valid
     * too. */
    if (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	if (wp->w_p_pvw || bt_quickfix(wp->w_buffer))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    for (;;)
	    {
		if (wp->w_next == NULL)
		    wp = firstwin;
		else
		    wp = wp->w_next;
		if (wp == curwin)
		    break;
		if (!wp->w_p_pvw && !bt_quickfix(wp->w_buffer))
		{
		    curwin = wp;
		    break;
		}
	    }
	}
#endif
	curbuf = curwin->w_buffer;
	close_curwin = TRUE;

	/* The cursor position may be invalid if the buffer changed after last
	 * using the window. */
	check_cursor();
    }
    if (p_ea && (*p_ead == 'b' || *p_ead == dir))
	/* If the frame of the closed window contains the new current window,
	 * only resize that frame.  Otherwise resize all windows. */
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    else
	win_comp_pos();
    if (close_curwin)
    {
	win_enter_ext(wp, FALSE, TRUE, FALSE, TRUE, TRUE);
	if (other_buffer)
	    /* careful: after this wp and win may be invalid! */
	    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
    }

    --split_disallowed;

    /*
     * If last window has a status line now and we don't want one,
     * remove the status line.
     */
    last_status(FALSE);

    /* After closing the help window, try restoring the window layout from
     * before it was opened. */
    if (help_window)
	restore_snapshot(SNAP_HELP_IDX, close_curwin);

#if defined(FEAT_GUI)
    /* When 'guioptions' includes 'L' or 'R' may have to remove scrollbars. */
    if (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(NULL);
#endif

    redraw_all_later(NOT_VALID);
    return OK;
}