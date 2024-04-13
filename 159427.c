nv_visual(cmdarg_T *cap)
{
    if (cap->cmdchar == Ctrl_Q)
	cap->cmdchar = Ctrl_V;

    // 'v', 'V' and CTRL-V can be used while an operator is pending to make it
    // characterwise, linewise, or blockwise.
    if (cap->oap->op_type != OP_NOP)
    {
	motion_force = cap->oap->motion_force = cap->cmdchar;
	finish_op = FALSE;	// operator doesn't finish now but later
	return;
    }

    VIsual_select = cap->arg;
    if (VIsual_active)	    // change Visual mode
    {
	if (VIsual_mode == cap->cmdchar)    // stop visual mode
	    end_visual_mode();
	else				    // toggle char/block mode
	{				    //	   or char/line mode
	    VIsual_mode = cap->cmdchar;
	    showmode();
	}
	redraw_curbuf_later(INVERTED);	    // update the inversion
    }
    else		    // start Visual mode
    {
	check_visual_highlight();
	if (cap->count0 > 0 && resel_VIsual_mode != NUL)
	{
	    // use previously selected part
	    VIsual = curwin->w_cursor;

	    VIsual_active = TRUE;
	    VIsual_reselect = TRUE;
	    if (!cap->arg)
		// start Select mode when 'selectmode' contains "cmd"
		may_start_select('c');
	    setmouse();
	    if (p_smd && msg_silent == 0)
		redraw_cmdline = TRUE;	    // show visual mode later
	    /*
	     * For V and ^V, we multiply the number of lines even if there
	     * was only one -- webb
	     */
	    if (resel_VIsual_mode != 'v' || resel_VIsual_line_count > 1)
	    {
		curwin->w_cursor.lnum +=
				    resel_VIsual_line_count * cap->count0 - 1;
		if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    }
	    VIsual_mode = resel_VIsual_mode;
	    if (VIsual_mode == 'v')
	    {
		if (resel_VIsual_line_count <= 1)
		{
		    validate_virtcol();
		    curwin->w_curswant = curwin->w_virtcol
					+ resel_VIsual_vcol * cap->count0 - 1;
		}
		else
		    curwin->w_curswant = resel_VIsual_vcol;
		coladvance(curwin->w_curswant);
	    }
	    if (resel_VIsual_vcol == MAXCOL)
	    {
		curwin->w_curswant = MAXCOL;
		coladvance((colnr_T)MAXCOL);
	    }
	    else if (VIsual_mode == Ctrl_V)
	    {
		validate_virtcol();
		curwin->w_curswant = curwin->w_virtcol
					+ resel_VIsual_vcol * cap->count0 - 1;
		coladvance(curwin->w_curswant);
	    }
	    else
		curwin->w_set_curswant = TRUE;
	    redraw_curbuf_later(INVERTED);	// show the inversion
	}
	else
	{
	    if (!cap->arg)
		// start Select mode when 'selectmode' contains "cmd"
		may_start_select('c');
	    n_start_visual_mode(cap->cmdchar);
	    if (VIsual_mode != 'V' && *p_sel == 'e')
		++cap->count1;  // include one more char
	    if (cap->count0 > 0 && --cap->count1 > 0)
	    {
		// With a count select that many characters or lines.
		if (VIsual_mode == 'v' || VIsual_mode == Ctrl_V)
		    nv_right(cap);
		else if (VIsual_mode == 'V')
		    nv_down(cap);
	    }
	}
    }
}