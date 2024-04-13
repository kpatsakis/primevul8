nv_put_opt(cmdarg_T *cap, int fix_indent)
{
    int		regname = 0;
    void	*reg1 = NULL, *reg2 = NULL;
    int		empty = FALSE;
    int		was_visual = FALSE;
    int		dir;
    int		flags = 0;

    if (cap->oap->op_type != OP_NOP)
    {
#ifdef FEAT_DIFF
	// "dp" is ":diffput"
	if (cap->oap->op_type == OP_DELETE && cap->cmdchar == 'p')
	{
	    clearop(cap->oap);
	    nv_diffgetput(TRUE, cap->opcount);
	}
	else
#endif
	clearopbeep(cap->oap);
    }
#ifdef FEAT_JOB_CHANNEL
    else if (bt_prompt(curbuf) && !prompt_curpos_editable())
    {
	clearopbeep(cap->oap);
    }
#endif
    else
    {
	if (fix_indent)
	{
	    dir = (cap->cmdchar == ']' && cap->nchar == 'p')
							 ? FORWARD : BACKWARD;
	    flags |= PUT_FIXINDENT;
	}
	else
	    dir = (cap->cmdchar == 'P'
		    || ((cap->cmdchar == 'g' || cap->cmdchar == 'z')
			&& cap->nchar == 'P')) ? BACKWARD : FORWARD;
	prep_redo_cmd(cap);
	if (cap->cmdchar == 'g')
	    flags |= PUT_CURSEND;
	else if (cap->cmdchar == 'z')
	    flags |= PUT_BLOCK_INNER;

	if (VIsual_active)
	{
	    // Putting in Visual mode: The put text replaces the selected
	    // text.  First delete the selected text, then put the new text.
	    // Need to save and restore the registers that the delete
	    // overwrites if the old contents is being put.
	    was_visual = TRUE;
	    regname = cap->oap->regname;
#ifdef FEAT_CLIPBOARD
	    adjust_clip_reg(&regname);
#endif
	   if (regname == 0 || regname == '"'
				     || VIM_ISDIGIT(regname) || regname == '-'
#ifdef FEAT_CLIPBOARD
		    || (clip_unnamed && (regname == '*' || regname == '+'))
#endif

		    )
	    {
		// The delete is going to overwrite the register we want to
		// put, save it first.
		reg1 = get_register(regname, TRUE);
	    }

	    // Now delete the selected text. Avoid messages here.
	    cap->cmdchar = 'd';
	    cap->nchar = NUL;
	    cap->oap->regname = NUL;
	    ++msg_silent;
	    nv_operator(cap);
	    do_pending_operator(cap, 0, FALSE);
	    empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
	    --msg_silent;

	    // delete PUT_LINE_BACKWARD;
	    cap->oap->regname = regname;

	    if (reg1 != NULL)
	    {
		// Delete probably changed the register we want to put, save
		// it first. Then put back what was there before the delete.
		reg2 = get_register(regname, FALSE);
		put_register(regname, reg1);
	    }

	    // When deleted a linewise Visual area, put the register as
	    // lines to avoid it joined with the next line.  When deletion was
	    // characterwise, split a line when putting lines.
	    if (VIsual_mode == 'V')
		flags |= PUT_LINE;
	    else if (VIsual_mode == 'v')
		flags |= PUT_LINE_SPLIT;
	    if (VIsual_mode == Ctrl_V && dir == FORWARD)
		flags |= PUT_LINE_FORWARD;
	    dir = BACKWARD;
	    if ((VIsual_mode != 'V'
			&& curwin->w_cursor.col < curbuf->b_op_start.col)
		    || (VIsual_mode == 'V'
			&& curwin->w_cursor.lnum < curbuf->b_op_start.lnum))
		// cursor is at the end of the line or end of file, put
		// forward.
		dir = FORWARD;
	    // May have been reset in do_put().
	    VIsual_active = TRUE;
	}
	do_put(cap->oap->regname, NULL, dir, cap->count1, flags);

	// If a register was saved, put it back now.
	if (reg2 != NULL)
	    put_register(regname, reg2);

	// What to reselect with "gv"?  Selecting the just put text seems to
	// be the most useful, since the original text was removed.
	if (was_visual)
	{
	    curbuf->b_visual.vi_start = curbuf->b_op_start;
	    curbuf->b_visual.vi_end = curbuf->b_op_end;
	    // need to adjust cursor position
	    if (*p_sel == 'e')
		inc(&curbuf->b_visual.vi_end);
	}

	// When all lines were selected and deleted do_put() leaves an empty
	// line that needs to be deleted now.
	if (empty && *ml_get(curbuf->b_ml.ml_line_count) == NUL)
	{
	    ml_delete_flags(curbuf->b_ml.ml_line_count, ML_DEL_MESSAGE);
	    deleted_lines(curbuf->b_ml.ml_line_count + 1, 1);

	    // If the cursor was in that line, move it to the end of the last
	    // line.
	    if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	    {
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		coladvance((colnr_T)MAXCOL);
	    }
	}
	auto_format(FALSE, TRUE);
    }
}