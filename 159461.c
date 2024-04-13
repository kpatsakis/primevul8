do_check_scrollbind(int check)
{
    static win_T	*old_curwin = NULL;
    static linenr_T	old_topline = 0;
#ifdef FEAT_DIFF
    static int		old_topfill = 0;
#endif
    static buf_T	*old_buf = NULL;
    static colnr_T	old_leftcol = 0;

    if (check && curwin->w_p_scb)
    {
	// If a ":syncbind" command was just used, don't scroll, only reset
	// the values.
	if (did_syncbind)
	    did_syncbind = FALSE;
	else if (curwin == old_curwin)
	{
	    /*
	     * Synchronize other windows, as necessary according to
	     * 'scrollbind'.  Don't do this after an ":edit" command, except
	     * when 'diff' is set.
	     */
	    if ((curwin->w_buffer == old_buf
#ifdef FEAT_DIFF
			|| curwin->w_p_diff
#endif
		)
		&& (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill != old_topfill
#endif
			|| curwin->w_leftcol != old_leftcol))
	    {
		check_scrollbind(curwin->w_topline - old_topline,
			(long)(curwin->w_leftcol - old_leftcol));
	    }
	}
	else if (vim_strchr(p_sbo, 'j')) // jump flag set in 'scrollopt'
	{
	    /*
	     * When switching between windows, make sure that the relative
	     * vertical offset is valid for the new window.  The relative
	     * offset is invalid whenever another 'scrollbind' window has
	     * scrolled to a point that would force the current window to
	     * scroll past the beginning or end of its buffer.  When the
	     * resync is performed, some of the other 'scrollbind' windows may
	     * need to jump so that the current window's relative position is
	     * visible on-screen.
	     */
	    check_scrollbind(curwin->w_topline - curwin->w_scbind_pos, 0L);
	}
	curwin->w_scbind_pos = curwin->w_topline;
    }

    old_curwin = curwin;
    old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    old_topfill = curwin->w_topfill;
#endif
    old_buf = curwin->w_buffer;
    old_leftcol = curwin->w_leftcol;
}