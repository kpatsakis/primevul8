shift_block(oparg_T *oap, int amount)
{
    int			left = (oap->op_type == OP_LSHIFT);
    int			oldstate = State;
    int			total;
    char_u		*newp, *oldp;
    int			oldcol = curwin->w_cursor.col;
    int			sw_val = (int)get_sw_value_indent(curbuf);
    int			ts_val = (int)curbuf->b_p_ts;
    struct block_def	bd;
    int			incr;
    colnr_T		ws_vcol;
    int			added;
    unsigned		new_line_len;	// the length of the line after the
					// block shift
#ifdef FEAT_RIGHTLEFT
    int			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = MODE_INSERT;	// don't want MODE_REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    if (bd.is_short)
	return;

    // total is number of screen columns to be inserted/removed
    total = (int)((unsigned)amount * (unsigned)sw_val);
    if ((total / sw_val) != amount)
	return; // multiplication overflow

    oldp = ml_get_curline();

    if (!left)
    {
	int		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	total += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.start_vcol - bd.pre_whitesp;
	if (bd.startspaces)
	{
	    if (has_mbyte)
	    {
		if ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		else
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    else
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.start_vcol, bd.textstart, bd.textstart);
	for ( ; VIM_ISWHITE(*cts.cts_ptr); )
	{
	    incr = lbr_chartabsize_adv(&cts);
	    total += incr;
	    cts.cts_vcol += incr;
	}
	bd.textstart = cts.cts_ptr;
	bd.start_vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#ifdef FEAT_VARTABS
	if (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + total,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	else
	    spaces = total;
#else
	if (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + total) / ts_val; // number of tabs
	if (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + total) % ts_val; // number of spp
	else
	    spaces = total;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_line_len = bd.textcol + tabs + spaces + (int)STRLEN(bd.textstart);
	newp = alloc(new_line_len + 1);
	if (newp == NULL)
	    return;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	vim_memset(newp + bd.textcol, TAB, (size_t)tabs);
	vim_memset(newp + bd.textcol + tabs, ' ', (size_t)spaces);
	// Note that STRMOVE() copies the trailing NUL.
	STRMOVE(newp + bd.textcol + tabs + spaces, bd.textstart);
    }
    else // left
    {
	colnr_T	    destination_col;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_end;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_width;// the (displayed) width of this part
					// of line
	unsigned    fill;		// nr of spaces that replace a TAB
	size_t	    block_space_width;
	size_t	    shift_amount;
	char_u	    *non_white = bd.textstart;
	colnr_T	    non_white_col;
	chartabsize_T cts;

	/*
	 * Firstly, let's find the first non-whitespace character that is
	 * displayed after the block's start column and the character's column
	 * number. Also, let's calculate the width of all the whitespace
	 * characters that are displayed in the block and precede the searched
	 * non-whitespace character.
	 */

	// If "bd.startspaces" is set, "bd.textstart" points to the character,
	// the part of which is displayed at the block's beginning. Let's start
	// searching from the next character.
	if (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_col, bd.textstart, non_white);
	while (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_col = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	block_space_width = non_white_col - oap->start_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	shift_amount = (block_space_width < (size_t)total
					 ? block_space_width : (size_t)total);

	// The column to which we will shift the text.
	destination_col = (colnr_T)(non_white_col - shift_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_end = bd.textstart;
	verbatim_copy_width = bd.start_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	if (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_width,
					     bd.textstart, verbatim_copy_end);
	while (cts.cts_vcol < destination_col)
	{
	    incr = lbr_chartabsize(&cts);
	    if (cts.cts_vcol + incr > destination_col)
		break;
	    cts.cts_vcol += incr;
	    MB_PTR_ADV(cts.cts_ptr);
	}
	verbatim_copy_width = cts.cts_vcol;
	verbatim_copy_end = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	// If "destination_col" is different from the width of the initial
	// part of the line that will be copied, it means we encountered a tab
	// character, which we will have to partly replace with spaces.
	fill = destination_col - verbatim_copy_width;

	// The replacement line will consist of:
	// - the beginning of the original line up to "verbatim_copy_end",
	// - "fill" number of spaces,
	// - the rest of the line, pointed to by non_white.
	new_line_len = (unsigned)(verbatim_copy_end - oldp)
		       + fill
		       + (unsigned)STRLEN(non_white);

	newp = alloc(new_line_len + 1);
	if (newp == NULL)
	    return;
	mch_memmove(newp, oldp, (size_t)(verbatim_copy_end - oldp));
	vim_memset(newp + (verbatim_copy_end - oldp), ' ', (size_t)fill);
	// Note that STRMOVE() copies the trailing NUL.
	STRMOVE(newp + (verbatim_copy_end - oldp) + fill, non_white);
    }
    // replace the line
    added = new_line_len - (int)STRLEN(oldp);
    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
    inserted_bytes(curwin->w_cursor.lnum, bd.textcol, added);
    State = oldstate;
    curwin->w_cursor.col = oldcol;
#ifdef FEAT_RIGHTLEFT
    p_ri = old_p_ri;
#endif
}