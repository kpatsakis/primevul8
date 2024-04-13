cursor_up(
    long	n,
    int		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum;

    if (n > 0)
    {
	lnum = curwin->w_cursor.lnum;
	// This fails if the cursor is already in the first line or the count
	// is larger than the line number and '-' is in 'cpoptions'
	if (lnum <= 1 || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != NULL))
	    return FAIL;
	if (n >= lnum)
	    lnum = 1;
	else
#ifdef FEAT_FOLDING
	    if (hasAnyFolding(curwin))
	{
	    /*
	     * Count each sequence of folded lines as one logical line.
	     */
	    // go to the start of the current fold
	    (void)hasFolding(lnum, &lnum, NULL);

	    while (n--)
	    {
		// move up one line
		--lnum;
		if (lnum <= 1)
		    break;
		// If we entered a fold, move to the beginning, unless in
		// Insert mode or when 'foldopen' contains "all": it will open
		// in a moment.
		if (n > 0 || !((State & INSERT) || (fdo_flags & FDO_ALL)))
		    (void)hasFolding(lnum, &lnum, NULL);
	    }
	    if (lnum < 1)
		lnum = 1;
	}
	else
#endif
	    lnum -= n;
	curwin->w_cursor.lnum = lnum;
    }

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    if (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    return OK;
}