op_delete(oparg_T *oap)
{
    int			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    struct block_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_line_count;
    int			did_yank = FALSE;

    if (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to do
	return OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    if (oap->empty)
	return u_save_cursor();

    if (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	return FAIL;
    }

    if (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ifdef FEAT_CLIPBOARD
    adjust_clip_reg(&oap->regname);
#endif

    if (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    if (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->block_mode
	    && oap->line_count > 1
	    && oap->motion_force == NUL
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col;
	if (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	if (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    if (       oap->motion_type == MCHAR
	    && oap->line_count == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	if (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    goto setmarks;
	if (vim_strchr(p_cpo, CPO_EMPTYREGION) != NULL)
	    beep_flush();
	return OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    if (oap->regname != '_')
    {
	if (oap->regname != 0)
	{
	    // check for read-only register
	    if (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
		return OK;
	    }
	    get_yank_register(oap->regname, TRUE); // yank into specif'd reg.
	    if (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	else
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	if (oap->motion_type == MLINE || oap->line_count > 1
							   || oap->use_reg_one)
	{
	    shift_delete_registers();
	    if (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	if ((
#ifdef FEAT_CLIPBOARD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#endif
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->line_count == 1)
	{
	    oap->regname = '-';
	    get_yank_register(oap->regname, TRUE);
	    if (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	if (!did_yank)
	{
	    int msg_silent_save = msg_silent;

	    msg_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    msg_silent = msg_silent_save;
	    if (n != 'y')
	    {
		emsg(_(e_command_aborted));
		return FAIL;
	    }
	}

#if defined(FEAT_EVAL)
	if (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#endif
    }

    /*
     * block mode delete
     */
    if (oap->block_mode)
    {
	if (u_save((linenr_T)(oap->start.lnum - 1),
			       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	    return FAIL;

	for (lnum = curwin->w_cursor.lnum; lnum <= oap->end.lnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, TRUE);
	    if (bd.textlen == 0)	// nothing to delete
		continue;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    if (lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cursor.col = bd.textcol + bd.startspaces;
		curwin->w_cursor.coladd = 0;
	    }

	    // "n" == number of chars deleted
	    // If we delete a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    n = bd.textlen - bd.startspaces - bd.endspaces;
	    oldp = ml_get(lnum);
	    newp = alloc(STRLEN(oldp) + 1 - n);
	    if (newp == NULL)
		continue;
	    // copy up to deleted part
	    mch_memmove(newp, oldp, (size_t)bd.textcol);
	    // insert spaces
	    vim_memset(newp + bd.textcol, ' ',
				     (size_t)(bd.startspaces + bd.endspaces));
	    // copy the part after the deleted part
	    oldp += bd.textcol + bd.textlen;
	    STRMOVE(newp + bd.textcol + bd.startspaces + bd.endspaces, oldp);
	    // replace the line
	    ml_replace(lnum, newp, FALSE);

#ifdef FEAT_PROP_POPUP
	    if (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#endif
	}

	check_cursor_col();
	changed_lines(curwin->w_cursor.lnum, curwin->w_cursor.col,
						       oap->end.lnum + 1, 0L);
	oap->line_count = 0;	    // no lines deleted
    }
    else if (oap->motion_type == MLINE)
    {
	if (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    if (oap->line_count > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_lines((long)(oap->line_count - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    if (u_save_cursor() == FAIL)
		return FAIL;
	    if (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_col = curwin->w_cursor.col;
	    }
	    else
		beginline(0);		    // cursor in column 0
	    truncate_line(FALSE);   // delete the rest of the line
				    // leave cursor past last char in line
	    if (oap->line_count > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	else
	{
	    del_lines(oap->line_count, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    else
    {
	if (virtual_op)
	{
	    int		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    if (gchar_pos(&oap->start) == '\t')
	    {
		if (u_save_cursor() == FAIL)	// save first line for undo
		    return FAIL;
		if (oap->line_count == 1)
		    endcol = getviscol2(oap->end.col, oap->end.coladd);
		coladvance_force(getviscol2(oap->start.col, oap->start.coladd));
		oap->start = curwin->w_cursor;
		if (oap->line_count == 1)
		{
		    coladvance(endcol);
		    oap->end.col = curwin->w_cursor.col;
		    oap->end.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->start;
		}
	    }

	    // Break a tab only when it's included in the area.
	    if (gchar_pos(&oap->end) == '\t'
				     && (int)oap->end.coladd < oap->inclusive)
	    {
		// save last line for undo
		if (u_save((linenr_T)(oap->end.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
		    return FAIL;
		curwin->w_cursor = oap->end;
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd));
		oap->end = curwin->w_cursor;
		curwin->w_cursor = oap->start;
	    }
	    if (has_mbyte)
		mb_adjust_opend(oap);
	}

	if (oap->line_count == 1)	// delete characters within one line
	{
	    if (u_save_cursor() == FAIL)	// save line for undo
		return FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    if (       vim_strchr(p_cpo, CPO_DOLLAR) != NULL
		    && oap->op_type == OP_CHANGE
		    && oap->end.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->end.col - !oap->inclusive);

	    n = oap->end.col - oap->start.col + 1 - !oap->inclusive;

	    if (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		char_u		*curline = ml_get_curline();
		int		len = (int)STRLEN(curline);

		if (oap->end.coladd != 0
			&& (int)oap->end.col >= len - 1
			&& !(oap->start.coladd && (int)oap->end.col >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		if (n == 0 && oap->start.coladd != oap->end.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		if (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (void)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	else				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    if (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->line_count)) == FAIL)
		return FAIL;

	    truncate_line(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_lines((long)(oap->line_count - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->end.col + 1 - !oap->inclusive);
	    curwin->w_cursor.col = 0;
	    (void)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (void)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	if (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_line_count - old_lcount);

setmarks:
    if ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	if (oap->block_mode)
	{
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = oap->start.col;
	}
	else
	    curbuf->b_op_end = oap->start;
	curbuf->b_op_start = oap->start;
    }

    return OK;
}