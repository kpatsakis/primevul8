check_scrollbind(linenr_T topline_diff, long leftcol_diff)
{
    int		want_ver;
    int		want_hor;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    int		old_VIsual_select = VIsual_select;
    int		old_VIsual_active = VIsual_active;
    colnr_T	tgt_leftcol = curwin->w_leftcol;
    long	topline;
    long	y;

    /*
     * check 'scrollopt' string for vertical and horizontal scroll options
     */
    want_ver = (vim_strchr(p_sbo, 'v') && topline_diff != 0);
#ifdef FEAT_DIFF
    want_ver |= old_curwin->w_p_diff;
#endif
    want_hor = (vim_strchr(p_sbo, 'h') && (leftcol_diff || topline_diff != 0));

    /*
     * loop through the scrollbound windows and scroll accordingly
     */
    VIsual_select = VIsual_active = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_buffer;
	// skip original window  and windows with 'noscrollbind'
	if (curwin != old_curwin && curwin->w_p_scb)
	{
	    /*
	     * do the vertical scroll
	     */
	    if (want_ver)
	    {
#ifdef FEAT_DIFF
		if (old_curwin->w_p_diff && curwin->w_p_diff)
		{
		    diff_set_topline(old_curwin, curwin);
		}
		else
#endif
		{
		    curwin->w_scbind_pos += topline_diff;
		    topline = curwin->w_scbind_pos;
		    if (topline > curbuf->b_ml.ml_line_count)
			topline = curbuf->b_ml.ml_line_count;
		    if (topline < 1)
			topline = 1;

		    y = topline - curwin->w_topline;
		    if (y > 0)
			scrollup(y, FALSE);
		    else
			scrolldown(-y, FALSE);
		}

		redraw_later(VALID);
		cursor_correct();
		curwin->w_redr_status = TRUE;
	    }

	    /*
	     * do the horizontal scroll
	     */
	    if (want_hor && curwin->w_leftcol != tgt_leftcol)
	    {
		curwin->w_leftcol = tgt_leftcol;
		leftcol_changed();
	    }
	}
    }

    /*
     * reset current-window
     */
    VIsual_select = old_VIsual_select;
    VIsual_active = old_VIsual_active;
    curwin = old_curwin;
    curbuf = old_curbuf;
}