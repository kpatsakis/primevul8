op_shift(oparg_T *oap, int curs_top, int amount)
{
    long	    i;
    int		    first_char;
    int		    block_col = 0;

    if (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	return;

    if (oap->block_mode)
	block_col = curwin->w_cursor.col;

    for (i = oap->line_count; --i >= 0; )
    {
	first_char = *ml_get_curline();
	if (first_char == NUL)				// empty line
	    curwin->w_cursor.col = 0;
	else if (oap->block_mode)
	    shift_block(oap, amount);
	else
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    if (first_char != '#' || !preprocs_left())
		shift_line(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    changed_lines(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    if (oap->block_mode)
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	curwin->w_cursor.col = block_col;
    }
    else if (curs_top)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    else
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#endif


    if (oap->line_count > p_report)
    {
	char	    *op;
	char	    *msg_line_single;
	char	    *msg_line_plural;

	if (oap->op_type == OP_RSHIFT)
	    op = ">";
	else
	    op = "<";
	msg_line_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	msg_line_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((char *)IObuff, IOSIZE,
		NGETTEXT(msg_line_single, msg_line_plural, oap->line_count),
		oap->line_count, op, amount);
	msg_attr_keep((char *)IObuff, 0, TRUE);
    }

    if ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = (colnr_T)STRLEN(ml_get(oap->end.lnum));
	if (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}