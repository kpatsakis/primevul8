do_join(
    long    count,
    int	    insert_space,
    int	    save_undo,
    int	    use_formatoptions UNUSED,
    int	    setmark)
{
    char_u	*curr = NULL;
    char_u      *curr_start = NULL;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    int		endcurr1 = NUL;
    int		endcurr2 = NUL;
    int		currsize = 0;	// size of the current line
    int		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	col = 0;
    int		ret = OK;
    int		*comments = NULL;
    int		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    int		prev_was_comment;
#ifdef FEAT_PROP_POPUP
    int		propcount = 0;	// number of props over all joined lines
    int		props_remaining;
#endif

    if (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + count)) == FAIL)
	return FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(count, TRUE);
    if (spaces == NULL)
	return FAIL;
    if (remove_comments)
    {
	comments = lalloc_clear(count * sizeof(int), TRUE);
	if (comments == NULL)
	{
	    vim_free(spaces);
	    return FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    for (t = 0; t < count; ++t)
    {
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
#ifdef FEAT_PROP_POPUP
	propcount += count_props((linenr_T) (curwin->w_cursor.lnum + t),
							t > 0, t + 1 == count);
#endif
	if (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_buffer->b_op_start.lnum = curwin->w_cursor.lnum;
	    curwin->w_buffer->b_op_start.col  = (colnr_T)STRLEN(curr);
	}
	if (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    if (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, insert_space,
							   &prev_was_comment);
		comments[t] = (int)(new_curr - curr);
		curr = new_curr;
	    }
	    else
		curr = skip_comment(curr, FALSE, insert_space,
							   &prev_was_comment);
	}

	if (insert_space && t > 0)
	{
	    curr = skipwhite(curr);
	    if (*curr != NUL && *curr != ')'
		    && sumsize != 0 && endcurr1 != TAB
		    && (!has_format_option(FO_MBYTE_JOIN)
			|| (mb_ptr2char(curr) < 0x100 && endcurr1 < 0x100))
		    && (!has_format_option(FO_MBYTE_JOIN2)
			|| (mb_ptr2char(curr) < 0x100
			    && !(enc_utf8 && utf_eat_space(endcurr1)))
			|| (endcurr1 < 0x100
			    && !(enc_utf8 && utf_eat_space(mb_ptr2char(curr)))))
	       )
	    {
		// don't add a space if the line is ending in a space
		if (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		else
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		if (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == NULL
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (int)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	if (insert_space && currsize > 0)
	{
	    if (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_BACK(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		if (cend > curr)
		{
		    MB_PTR_BACK(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    else
	    {
		endcurr1 = *(curr + currsize - 1);
		if (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	line_breakcheck();
	if (got_int)
	{
	    ret = FAIL;
	    goto theend;
	}
    }

    // store the column position before last line
    col = sumsize - currsize - spaces[count - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
#ifdef FEAT_PROP_POPUP
    newp_len += propcount * sizeof(textprop_T);
#endif
    newp = alloc(newp_len);
    if (newp == NULL)
    {
	ret = FAIL;
	goto theend;
    }
    cend = newp + sumsize;
    *cend = 0;

    /*
     * Move affected lines to the new long one.
     * This loops backwards over the joined lines, including the original line.
     *
     * Move marks from each deleted line to the joined line, adjusting the
     * column.  This is not Vi compatible, but Vi deletes the marks, thus that
     * should not really be a problem.
     */
#ifdef FEAT_PROP_POPUP
    props_remaining = propcount;
#endif
    for (t = count - 1; ; --t)
    {
	int spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	if (spaces[t] > 0)
	{
	    cend -= spaces[t];
	    vim_memset(cend, ' ', (size_t)(spaces[t]));
	}

	// If deleting more spaces than adding, the cursor moves no more than
	// what is added if it is inside these spaces.
	spaces_removed = (curr - curr_start) - spaces[t];

	mark_col_adjust(curwin->w_cursor.lnum + t, (colnr_T)0, -t,
			 (long)(cend - newp - spaces_removed), spaces_removed);
#ifdef FEAT_PROP_POPUP
	prepend_joined_props(newp + sumsize + 1, propcount, &props_remaining,
		curwin->w_cursor.lnum + t, t == count - 1,
		(long)(cend - newp), spaces_removed);
#endif
	if (t == 0)
	    break;
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	if (remove_comments)
	    curr += comments[t - 1];
	if (insert_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (int)STRLEN(curr);
    }

    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    if (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_buffer->b_op_end.lnum = curwin->w_cursor.lnum;
	curwin->w_buffer->b_op_end.col  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    changed_lines(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_lines(count - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.col =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != NULL ? currsize : col);
    check_cursor_col();

    curwin->w_cursor.coladd = 0;
    curwin->w_set_curswant = TRUE;

theend:
    vim_free(spaces);
    if (remove_comments)
	vim_free(comments);
    return ret;
}