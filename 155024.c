scroll_to_fraction(win_T *wp, int prev_height)
{
    linenr_T	lnum;
    int		sline, line_size;
    int		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    if (height > 0
        && (!wp->w_p_scb || wp == curwin)
        && (height < wp->w_buffer->b_ml.ml_line_count || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	if (lnum < 1)		/* can happen when starting up */
	    lnum = 1;
	wp->w_wrow = ((long)wp->w_fraction * (long)height - 1L)
							       / FRACTION_MULT;
	line_size = plines_win_col(wp, lnum, (long)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - line_size;

	if (sline >= 0)
	{
	    /* Make sure the whole cursor line is visible, if possible. */
	    int rows = plines_win(wp, lnum, FALSE);

	    if (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - line_size;
	    }
	}

	if (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    wp->w_wrow = line_size;
	    if (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		wp->w_skipcol += wp->w_width - win_col_off(wp);
		--wp->w_wrow;
		while (wp->w_wrow >= wp->w_height)
		{
		    wp->w_skipcol += wp->w_width - win_col_off(wp)
							   + win_col_off2(wp);
		    --wp->w_wrow;
		}
	    }
	}
	else if (sline > 0)
	{
	    while (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);
		if (lnum == 1)
		{
		    /* first line in buffer is folded */
		    line_size = 1;
		    --sline;
		    break;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		if (lnum == wp->w_topline)
		    line_size = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		else
#endif
		    line_size = plines_win(wp, lnum, TRUE);
		sline -= line_size;
	    }

	    if (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, NULL, &lnum, TRUE, NULL);
#endif
		lnum++;
		wp->w_wrow -= line_size + sline;
	    }
	    else if (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    if (wp == curwin)
    {
	if (get_scrolloff_value())
	    update_topline();
	curs_columns(FALSE);	/* validate w_wrow */
    }
    if (prev_height > 0)
	wp->w_prev_fraction_row = wp->w_wrow;

    win_comp_scroll(wp);
    redraw_win_later(wp, SOME_VALID);
    wp->w_redr_status = TRUE;
    invalidate_botline_win(wp);
}