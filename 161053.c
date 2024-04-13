ins_tab(void)
{
    int		ind;
    int		i;
    int		temp;

    if (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    if (echeck_abbr(TAB + ABBR_OFF))
	return FALSE;

    ind = inindent(0);
#ifdef FEAT_CINDENT
    if (ind)
	can_cindent = FALSE;
#endif

    /*
     * When nothing special, insert TAB like a normal character.
     */
    if (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_count(curbuf->b_p_vts_array) > 1)
		    || (tabstop_count(curbuf->b_p_vts_array) == 1
		        && tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_value(curbuf))
	            || (tabstop_count(curbuf->b_p_vts_array) == 0
		        && curbuf->b_p_ts != get_sw_value(curbuf))))
	    && tabstop_count(curbuf->b_p_vsts_array) == 0
#else
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_value(curbuf))
#endif
	    && get_sts_value() == 0)
	return TRUE;

    if (stop_arrow() == FAIL)
	return TRUE;

    did_ai = FALSE;
#ifdef FEAT_SMARTINDENT
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
#endif
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    if (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	temp = (int)get_sw_value(curbuf);
	temp -= get_nolist_virtcol() % temp;
    }
    else if (tabstop_count(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
	                        // use 'softtabstop' when set
	temp = tabstop_padding(get_nolist_virtcol(), get_sts_value(),
						     curbuf->b_p_vsts_array);
    else			// otherwise use 'tabstop'
	temp = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#else
    if (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	temp = (int)get_sw_value(curbuf);
    else if (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	temp = (int)get_sts_value();
    else			// otherwise use 'tabstop'
	temp = (int)curbuf->b_p_ts;
    temp -= get_nolist_virtcol() % temp;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For VREPLACE mode, we use ins_char() for all characters.
     */
    ins_char(' ');
    while (--temp > 0)
    {
	if (State & VREPLACE_FLAG)
	    ins_char(' ');
	else
	{
	    ins_str((char_u *)" ");
	    if (State & REPLACE_FLAG)	    // no char replaced
		replace_push(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    if (!curbuf->b_p_et && (tabstop_count(curbuf->b_p_vsts_array) > 0
                            || get_sts_value() > 0
			    || (p_sta && ind)))
#else
    if (!curbuf->b_p_et && (get_sts_value() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_line = NULL;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	int		change_col = -1;
	int		save_list = curwin->w_p_list;

	/*
	 * Get the current line.  For VREPLACE mode, don't make real changes
	 * yet, just work on a copy of the line.
	 */
	if (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_line = vim_strsave(ml_get_curline());
	    if (saved_line == NULL)
		return FALSE;
	    ptr = saved_line + pos.col;
	}
	else
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	if (vim_strchr(p_cpo, CPO_LISTWM) == NULL)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	while (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	if ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.col < Insstart.col)
	{
	    ptr += Insstart.col - fpos.col;
	    fpos.col = Insstart.col;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, NULL, NULL);
	getvcol(curwin, cursor, &want_vcol, NULL, NULL);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	while (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(NULL, (char_u *)"\t", vcol);
	    if (vcol + i > want_vcol)
		break;
	    if (*ptr != TAB)
	    {
		*ptr = TAB;
		if (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    if (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    vcol += i;
	}

	if (change_col >= 0)
	{
	    int repl_off = 0;
	    char_u *line = ptr;

	    // Skip over the spaces we need.
	    while (vcol < want_vcol && *ptr == ' ')
	    {
		vcol += lbr_chartabsize(line, ptr, vcol);
		++ptr;
		++repl_off;
	    }
	    if (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    if (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		if (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    int	    col;

		    newp = alloc(curbuf->b_ml.ml_line_len - i);
		    if (newp == NULL)
			return FALSE;

		    col = ptr - curbuf->b_ml.ml_line_ptr;
		    if (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_line_len - col - i);

		    if (curbuf->b_ml.ml_flags & ML_LINE_DIRTY)
			vim_free(curbuf->b_ml.ml_line_ptr);
		    curbuf->b_ml.ml_line_ptr = newp;
		    curbuf->b_ml.ml_line_len -= i;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_LINE_DIRTY) & ~ML_EMPTY;
		}
		else
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		if ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    for (temp = i; --temp >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    if (netbeans_active())
	    {
		netbeans_removed(curbuf, fpos.lnum, cursor->col, (long)(i + 1));
		netbeans_inserted(curbuf, fpos.lnum, cursor->col,
							   (char_u *)"\t", 1);
	    }
#endif
	    cursor->col -= i;

	    /*
	     * In VREPLACE mode, we haven't changed anything yet.  Do it now by
	     * backspacing over the changed spacing and then inserting the new
	     * spacing.
	     */
	    if (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_line + change_col,
						    cursor->col - change_col);
	    }
	}

	if (State & VREPLACE_FLAG)
	    vim_free(saved_line);
	curwin->w_p_list = save_list;
    }

    return FALSE;
}