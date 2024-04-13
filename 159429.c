find_decl(
    char_u	*ptr,
    int		len,
    int		locally,
    int		thisblock,
    int		flags_arg)	// flags passed to searchit()
{
    char_u	*pat;
    pos_T	old_pos;
    pos_T	par_pos;
    pos_T	found_pos;
    int		t;
    int		save_p_ws;
    int		save_p_scs;
    int		retval = OK;
    int		incll;
    int		searchflags = flags_arg;
    int		valid;

    if ((pat = alloc(len + 7)) == NULL)
	return FAIL;

    // Put "\V" before the pattern to avoid that the special meaning of "."
    // and "~" causes trouble.
    sprintf((char *)pat, vim_iswordp(ptr) ? "\\V\\<%.*s\\>" : "\\V%.*s",
								    len, ptr);
    old_pos = curwin->w_cursor;
    save_p_ws = p_ws;
    save_p_scs = p_scs;
    p_ws = FALSE;	// don't wrap around end of file now
    p_scs = FALSE;	// don't switch ignorecase off now

    /*
     * With "gD" go to line 1.
     * With "gd" Search back for the start of the current function, then go
     * back until a blank line.  If this fails go to line 1.
     */
    if (!locally || !findpar(&incll, BACKWARD, 1L, '{', FALSE))
    {
	setpcmark();			// Set in findpar() otherwise
	curwin->w_cursor.lnum = 1;
	par_pos = curwin->w_cursor;
    }
    else
    {
	par_pos = curwin->w_cursor;
	while (curwin->w_cursor.lnum > 1 && *skipwhite(ml_get_curline()) != NUL)
	    --curwin->w_cursor.lnum;
    }
    curwin->w_cursor.col = 0;

    // Search forward for the identifier, ignore comment lines.
    CLEAR_POS(&found_pos);
    for (;;)
    {
	t = searchit(curwin, curbuf, &curwin->w_cursor, NULL, FORWARD,
					  pat, 1L, searchflags, RE_LAST, NULL);
	if (curwin->w_cursor.lnum >= old_pos.lnum)
	    t = FAIL;	// match after start is failure too

	if (thisblock && t != FAIL)
	{
	    pos_T	*pos;

	    // Check that the block the match is in doesn't end before the
	    // position where we started the search from.
	    if ((pos = findmatchlimit(NULL, '}', FM_FORWARD,
		     (int)(old_pos.lnum - curwin->w_cursor.lnum + 1))) != NULL
		    && pos->lnum < old_pos.lnum)
	    {
		// There can't be a useful match before the end of this block.
		// Skip to the end.
		curwin->w_cursor = *pos;
		continue;
	    }
	}

	if (t == FAIL)
	{
	    // If we previously found a valid position, use it.
	    if (found_pos.lnum != 0)
	    {
		curwin->w_cursor = found_pos;
		t = OK;
	    }
	    break;
	}
	if (get_leader_len(ml_get_curline(), NULL, FALSE, TRUE) > 0)
	{
	    // Ignore this line, continue at start of next line.
	    ++curwin->w_cursor.lnum;
	    curwin->w_cursor.col = 0;
	    continue;
	}
	valid = is_ident(ml_get_curline(), curwin->w_cursor.col);

	// If the current position is not a valid identifier and a previous
	// match is present, favor that one instead.
	if (!valid && found_pos.lnum != 0)
	{
	    curwin->w_cursor = found_pos;
	    break;
	}

	// Global search: use first valid match found
	if (valid && !locally)
	    break;
	if (valid && curwin->w_cursor.lnum >= par_pos.lnum)
	{
	    // If we previously found a valid position, use it.
	    if (found_pos.lnum != 0)
		curwin->w_cursor = found_pos;
	    break;
	}

	// For finding a local variable and the match is before the "{" or
	// inside a comment, continue searching.  For K&R style function
	// declarations this skips the function header without types.
	if (!valid)
	    CLEAR_POS(&found_pos);
	else
	    found_pos = curwin->w_cursor;
	// Remove SEARCH_START from flags to avoid getting stuck at one
	// position.
	searchflags &= ~SEARCH_START;
    }

    if (t == FAIL)
    {
	retval = FAIL;
	curwin->w_cursor = old_pos;
    }
    else
    {
	curwin->w_set_curswant = TRUE;
	// "n" searches forward now
	reset_search_dir();
    }

    vim_free(pat);
    p_ws = save_p_ws;
    p_scs = save_p_scs;

    return retval;
}