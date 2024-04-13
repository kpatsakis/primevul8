do_pending_operator(cmdarg_T *cap, int old_col, int gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    int		empty_region_error;
    int		restart_edit_save;
#ifdef FEAT_LINEBREAK
    int		lbr_saved = curwin->w_p_lbr;
#endif

    // The visual area is remembered for redo
    static redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    int		    include_line_break = FALSE;

#if defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    if ((clip_star.available || clip_plus.available)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_active
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    if ((finish_op || VIsual_active) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	int	redo_yank = vim_strchr(p_cpo, CPO_YANK) != NULL && !gui_yank;

#ifdef FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	if (curwin->w_p_lbr)
	    curwin->w_valid &= ~VALID_VIRTCOL;
	curwin->w_p_lbr = FALSE;
#endif
	oap->is_VIsual = VIsual_active;
	if (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	else if (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    if (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    else if (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	else if (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    if (!VIsual_active)
	    {
		VIsual_active = TRUE;
		VIsual = oap->start;
	    }
	    VIsual_mode = Ctrl_V;
	    VIsual_select = FALSE;
	    VIsual_reselect = FALSE;
	}

	// Only redo yank when 'y' flag is in 'cpoptions'.
	// Never redo "zf" (define fold).
	if ((redo_yank || oap->op_type != OP_YANK)
		&& ((!VIsual_active || oap->motion_force)
		    // Also redo Operator-pending Visual mode mappings
		    || (VIsual_active
			    && is_ex_cmdchar(cap) && oap->op_type != OP_COLON))
		&& cap->cmdchar != 'D'
#ifdef FEAT_FOLDING
		&& oap->op_type != OP_FOLD
		&& oap->op_type != OP_FOLDOPEN
		&& oap->op_type != OP_FOLDOPENREC
		&& oap->op_type != OP_FOLDCLOSE
		&& oap->op_type != OP_FOLDCLOSEREC
		&& oap->op_type != OP_FOLDDEL
		&& oap->op_type != OP_FOLDDELREC
#endif
		)
	{
	    prep_redo(oap->regname, cap->count0,
		    get_op_char(oap->op_type), get_extra_op_char(oap->op_type),
		    oap->motion_force, cap->cmdchar, cap->nchar);
	    if (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		if (vim_strchr(p_cpo, CPO_REDO) == NULL)
		    AppendToRedobuffLit(cap->searchbuf, -1);
		AppendToRedobuff(NL_STR);
	    }
	    else if (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		if (repeat_cmdline == NULL)
		    ResetRedobuff();
		else
		{
		    AppendToRedobuffLit(repeat_cmdline, -1);
		    AppendToRedobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	if (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_line_count - 1;
	    if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    VIsual_mode = redo_VIsual.rv_mode;
	    if (redo_VIsual.rv_vcol == MAXCOL || VIsual_mode == 'v')
	    {
		if (VIsual_mode == 'v')
		{
		    if (redo_VIsual.rv_line_count <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    else
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		else
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_count;
	    if (redo_VIsual.rv_count != 0)
		cap->count1 = redo_VIsual.rv_count;
	    else
		cap->count1 = 1;
	}
	else if (VIsual_active)
	{
	    if (!gui_yank)
	    {
		// Save the current VIsual area for '< and '> marks, and "gv"
		curbuf->b_visual.vi_start = VIsual;
		curbuf->b_visual.vi_end = curwin->w_cursor;
		curbuf->b_visual.vi_mode = VIsual_mode;
		restore_visual_mode();
		curbuf->b_visual.vi_curswant = curwin->w_curswant;
#ifdef FEAT_EVAL
		curbuf->b_visual_mode_eval = VIsual_mode;
#endif
	    }

	    // In Select mode, a linewise selection is operated upon like a
	    // characterwise selection.
	    // Special case: gH<Del> deletes the last line.
	    if (VIsual_select && VIsual_mode == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		if (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.col = 0;
		    curwin->w_cursor.col =
			       (colnr_T)STRLEN(ml_get(curwin->w_cursor.lnum));
		}
		else
		{
		    curwin->w_cursor.col = 0;
		    VIsual.col = (colnr_T)STRLEN(ml_get(VIsual.lnum));
		}
		VIsual_mode = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    else if (VIsual_mode == 'v')
		include_line_break = unadjust_for_sel();

	    oap->start = VIsual;
	    if (VIsual_mode == 'V')
	    {
		oap->start.col = 0;
		oap->start.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	if (LT_POS(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    if (!VIsual_active)
	    {
		if (hasFolding(oap->start.lnum, &oap->start.lnum, NULL))
		    oap->start.col = 0;
		if ((curwin->w_cursor.col > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, NULL,
						      &curwin->w_cursor.lnum))
		    curwin->w_cursor.col = (colnr_T)STRLEN(ml_get_curline());
	    }
#endif
	    oap->end = curwin->w_cursor;
	    curwin->w_cursor = oap->start;

	    // w_virtcol may have been updated; if the cursor goes back to its
	    // previous position w_virtcol becomes invalid and isn't updated
	    // automatically.
	    curwin->w_valid &= ~VALID_VIRTCOL;
	}
	else
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    if (!VIsual_active && oap->motion_type == MLINE)
	    {
		if (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									NULL))
		    curwin->w_cursor.col = 0;
		if (hasFolding(oap->start.lnum, NULL, &oap->start.lnum))
		    oap->start.col = (colnr_T)STRLEN(ml_get(oap->start.lnum));
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_buffer, &oap->end);
	oap->line_count = oap->end.lnum - oap->start.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_active();

	if (VIsual_active || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    if (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_mode = VIsual_mode;
		if (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		else
		{
		    if (VIsual_mode != Ctrl_V)
			getvvcol(curwin, &(oap->end),
						  NULL, NULL, &oap->end_vcol);
		    if (VIsual_mode == Ctrl_V || oap->line_count <= 1)
		    {
			if (VIsual_mode != Ctrl_V)
			    getvvcol(curwin, &(oap->start),
						&oap->start_vcol, NULL, NULL);
			resel_VIsual_vcol = oap->end_vcol - oap->start_vcol + 1;
		    }
		    else
			resel_VIsual_vcol = oap->end_vcol;
		}
		resel_VIsual_line_count = oap->line_count;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    if ((redo_yank || oap->op_type != OP_YANK)
		    && oap->op_type != OP_COLON
#ifdef FEAT_FOLDING
		    && oap->op_type != OP_FOLD
		    && oap->op_type != OP_FOLDOPEN
		    && oap->op_type != OP_FOLDOPENREC
		    && oap->op_type != OP_FOLDCLOSE
		    && oap->op_type != OP_FOLDCLOSEREC
		    && oap->op_type != OP_FOLDDEL
		    && oap->op_type != OP_FOLDDELREC
#endif
		    && oap->motion_force == NUL
		    )
	    {
		// Prepare for redoing.  Only use the nchar field for "r",
		// otherwise it might be the second char of the operator.
		if (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		else if (!is_ex_cmdchar(cap))
		{
		    int opchar = get_op_char(oap->op_type);
		    int extra_opchar = get_extra_op_char(oap->op_type);
		    int nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    if (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    else if (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    if (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    else
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		if (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_mode = resel_VIsual_mode;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_line_count = resel_VIsual_line_count;
		    redo_VIsual.rv_count = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    if (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    if (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    else
	    {
		oap->motion_type = MCHAR;
		if (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL
			&& (include_line_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    if (*p_sel != 'o'
			    && !op_on_lines(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_line_count)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			oap->end.coladd = 0;
			++oap->line_count;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    if (!gui_yank)
	    {
		VIsual_active = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		if ((oap->op_type == OP_YANK
			    || oap->op_type == OP_COLON
			    || oap->op_type == OP_FUNCTION
			    || oap->op_type == OP_FILTER)
			&& oap->motion_force == NUL)
		{
#ifdef FEAT_LINEBREAK
		    // make sure redrawing is correct
		    curwin->w_p_lbr = lbr_saved;
#endif
		    redraw_curbuf_later(UPD_INVERTED);
		}
	    }
	}

	// Include the trailing byte of a multi-byte char.
	if (has_mbyte && oap->inclusive)
	{
	    int		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->end));
	    if (l > 1)
		oap->end.col += l - 1;
	}
	curwin->w_set_curswant = TRUE;

	// oap->empty is set when start and end are the same.  The inclusive
	// flag affects this too, unless yanking and the end is on a NUL.
	oap->empty = (oap->motion_type == MCHAR
		    && (!oap->inclusive
			|| (oap->op_type == OP_YANK
			    && gchar_pos(&oap->end) == NUL))
		    && EQUAL_POS(oap->start, oap->end)
		    && !(virtual_op && oap->start.coladd != oap->end.coladd));
	// For delete, change and yank, it's an error to operate on an
	// empty region, when 'E' included in 'cpoptions' (Vi compatible).
	empty_region_error = (oap->empty
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != NULL);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	if (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma
#ifdef FEAT_FOLDING
		    || oap->op_type == OP_FOLD
#endif
		    ))
	{
#ifdef FEAT_LINEBREAK
	    curwin->w_p_lbr = lbr_saved;
#endif
	    redraw_curbuf_later(UPD_INVERTED);
	}

	// If the end of an operator is in column one while oap->motion_type
	// is MCHAR and oap->inclusive is FALSE, we put op_end after the last
	// character in the previous line. If op_start is on or before the
	// first non-blank in the line, the operator becomes linewise
	// (strange, but that's the way vi does it).
	if (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->block_mode
		&& oap->line_count > 1)
	{
	    oap->end_adjusted = TRUE;	    // remember that we did this
	    --oap->line_count;
	    --oap->end.lnum;
	    if (inindent(0))
		oap->motion_type = MLINE;
	    else
	    {
		oap->end.col = (colnr_T)STRLEN(ml_get(oap->end.lnum));
		if (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	else
	    oap->end_adjusted = FALSE;

	switch (oap->op_type)
	{
	case OP_LSHIFT:
	case OP_RSHIFT:
	    op_shift(oap, TRUE, oap->is_VIsual ? (int)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    break;

	case OP_JOIN_NS:
	case OP_JOIN:
	    if (oap->line_count < 2)
		oap->line_count = 2;
	    if (curwin->w_cursor.lnum + oap->line_count - 1 >
						   curbuf->b_ml.ml_line_count)
		beep_flush();
	    else
	    {
		(void)do_join(oap->line_count, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    break;

	case OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    if (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    else
	    {
		(void)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		if (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    break;

	case OP_YANK:
	    if (empty_region_error)
	    {
		if (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    else
	    {
#ifdef FEAT_LINEBREAK
		curwin->w_p_lbr = lbr_saved;
#endif
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(void)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_col();
	    break;

	case OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    if (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    else
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		if (p_im || !KeyTyped)
		    restart_edit_save = restart_edit;
		else
		    restart_edit_save = 0;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		curwin->w_p_lbr = lbr_saved;
#endif
		// Reset finish_op now, don't want it set inside edit().
		finish_op = FALSE;
		if (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		if (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    break;

	case OP_FILTER:
	    if (vim_strchr(p_cpo, CPO_FILTER) != NULL)
		AppendToRedobuff((char_u *)"!\r");  // use any last used !cmd
	    else
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	case OP_INDENT:
	case OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    if (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		if (curbuf->b_p_lisp)
		{
		    op_reindent(oap, get_lisp_indent);
		    break;
		}
		op_reindent(oap,
#ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_expr_indent :
#endif
			    get_c_indent);
		break;
	    }

	    op_colon(oap);
	    break;

	case OP_TILDE:
	case OP_UPPER:
	case OP_LOWER:
	case OP_ROT13:
	    if (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    else
		op_tilde(oap);
	    check_cursor_col();
	    break;

	case OP_FORMAT:
#if defined(FEAT_EVAL)
	    if (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    else
#endif
	    {
		if (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		else
		    op_format(oap, FALSE);	// use internal function
	    }
	    break;
	case OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    break;

	case OP_FUNCTION:
	    {
		redo_VIsual_T   save_redo_VIsual = redo_VIsual;

#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		curwin->w_p_lbr = lbr_saved;
#endif
		// call 'operatorfunc'
		op_function(oap);

		// Restore the info for redoing Visual mode, the function may
		// invoke another operator and unintentionally change it.
		redo_VIsual = save_redo_VIsual;
		break;
	    }

	case OP_INSERT:
	case OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    if (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    else
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once.
		restart_edit_save = restart_edit;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		curwin->w_p_lbr = lbr_saved;
#endif
		op_insert(oap, cap->count1);
#ifdef FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		curwin->w_p_lbr = FALSE;
#endif

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		if (restart_edit == 0)
		    restart_edit = restart_edit_save;
		else
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    break;

	case OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    if (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    else
	    {
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		curwin->w_p_lbr = lbr_saved;
#endif
		op_replace(oap, cap->nchar);
	    }
	    break;

#ifdef FEAT_FOLDING
	case OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->start.lnum, oap->end.lnum);
	    break;

	case OP_FOLDOPEN:
	case OP_FOLDOPENREC:
	case OP_FOLDCLOSE:
	case OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->start.lnum, oap->end.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    break;

	case OP_FOLDDEL:
	case OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->start.lnum, oap->end.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    break;
#endif
	case OP_NR_ADD:
	case OP_NR_SUB:
	    if (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    else
	    {
		VIsual_active = TRUE;
#ifdef FEAT_LINEBREAK
		curwin->w_p_lbr = lbr_saved;
#endif
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_active = FALSE;
	    }
	    check_cursor_col();
	    break;
	default:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	if (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    if (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ifdef FEAT_LINEBREAK
		curwin->w_p_lbr = FALSE;
#endif
		coladvance(curwin->w_curswant = old_col);
	    }
	}
	else
	{
	    curwin->w_cursor = old_cursor;
	}
	oap->block_mode = FALSE;
	clearop(oap);
	motion_force = NUL;
    }
#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = lbr_saved;
#endif
}