nv_zet(cmdarg_T *cap)
{
    long	n;
    colnr_T	col;
    int		nchar = cap->nchar;
#ifdef FEAT_FOLDING
    long	old_fdl = curwin->w_p_fdl;
    int		old_fen = curwin->w_p_fen;
#endif
#ifdef FEAT_SPELL
    int		undo = FALSE;
#endif
    long        siso = get_sidescrolloff_value();

    if (VIM_ISDIGIT(nchar))
    {
	/*
	 * "z123{nchar}": edit the count before obtaining {nchar}
	 */
	if (checkclearop(cap->oap))
	    return;
	n = nchar - '0';
	for (;;)
	{
#ifdef USE_ON_FLY_SCROLL
	    dont_scroll = TRUE;		// disallow scrolling here
#endif
	    ++no_mapping;
	    ++allow_keys;   // no mapping for nchar, but allow key codes
	    nchar = plain_vgetc();
	    LANGMAP_ADJUST(nchar, TRUE);
	    --no_mapping;
	    --allow_keys;
#ifdef FEAT_CMDL_INFO
	    (void)add_to_showcmd(nchar);
#endif
	    if (nchar == K_DEL || nchar == K_KDEL)
		n /= 10;
	    else if (VIM_ISDIGIT(nchar))
		n = n * 10 + (nchar - '0');
	    else if (nchar == CAR)
	    {
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight((int)n);
		break;
	    }
	    else if (nchar == 'l'
		    || nchar == 'h'
		    || nchar == K_LEFT
		    || nchar == K_RIGHT)
	    {
		cap->count1 = n ? n * cap->count1 : cap->count1;
		goto dozet;
	    }
	    else
	    {
		clearopbeep(cap->oap);
		break;
	    }
	}
	cap->oap->op_type = OP_NOP;
	return;
    }

dozet:
    if (
#ifdef FEAT_FOLDING
	    // "zf" and "zF" are always an operator, "zd", "zo", "zO", "zc"
	    // and "zC" only in Visual mode.  "zj" and "zk" are motion
	    // commands.
	    cap->nchar != 'f' && cap->nchar != 'F'
	    && !(VIsual_active && vim_strchr((char_u *)"dcCoO", cap->nchar))
	    && cap->nchar != 'j' && cap->nchar != 'k'
	    &&
#endif
	    checkclearop(cap->oap))
	return;

    /*
     * For "z+", "z<CR>", "zt", "z.", "zz", "z^", "z-", "zb":
     * If line number given, set cursor.
     */
    if ((vim_strchr((char_u *)"+\r\nt.z^-b", nchar) != NULL)
	    && cap->count0
	    && cap->count0 != curwin->w_cursor.lnum)
    {
	setpcmark();
	if (cap->count0 > curbuf->b_ml.ml_line_count)
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	else
	    curwin->w_cursor.lnum = cap->count0;
	check_cursor_col();
    }

    switch (nchar)
    {
		// "z+", "z<CR>" and "zt": put cursor at top of screen
    case '+':
		if (cap->count0 == 0)
		{
		    // No count given: put cursor at the line below screen
		    validate_botline();	// make sure w_botline is valid
		    if (curwin->w_botline > curbuf->b_ml.ml_line_count)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		    else
			curwin->w_cursor.lnum = curwin->w_botline;
		}
		// FALLTHROUGH
    case NL:
    case CAR:
    case K_KENTER:
		beginline(BL_WHITE | BL_FIX);
		// FALLTHROUGH

    case 't':	scroll_cursor_top(0, TRUE);
		redraw_later(VALID);
		set_fraction(curwin);
		break;

		// "z." and "zz": put cursor in middle of screen
    case '.':	beginline(BL_WHITE | BL_FIX);
		// FALLTHROUGH

    case 'z':	scroll_cursor_halfway(TRUE);
		redraw_later(VALID);
		set_fraction(curwin);
		break;

		// "z^", "z-" and "zb": put cursor at bottom of screen
    case '^':	// Strange Vi behavior: <count>z^ finds line at top of window
		// when <count> is at bottom of window, and puts that one at
		// bottom of window.
		if (cap->count0 != 0)
		{
		    scroll_cursor_bot(0, TRUE);
		    curwin->w_cursor.lnum = curwin->w_topline;
		}
		else if (curwin->w_topline == 1)
		    curwin->w_cursor.lnum = 1;
		else
		    curwin->w_cursor.lnum = curwin->w_topline - 1;
		// FALLTHROUGH
    case '-':
		beginline(BL_WHITE | BL_FIX);
		// FALLTHROUGH

    case 'b':	scroll_cursor_bot(0, TRUE);
		redraw_later(VALID);
		set_fraction(curwin);
		break;

		// "zH" - scroll screen right half-page
    case 'H':
		cap->count1 *= curwin->w_width / 2;
		// FALLTHROUGH

		// "zh" - scroll screen to the right
    case 'h':
    case K_LEFT:
		if (!curwin->w_p_wrap)
		{
		    if ((colnr_T)cap->count1 > curwin->w_leftcol)
			curwin->w_leftcol = 0;
		    else
			curwin->w_leftcol -= (colnr_T)cap->count1;
		    leftcol_changed();
		}
		break;

		// "zL" - scroll screen left half-page
    case 'L':	cap->count1 *= curwin->w_width / 2;
		// FALLTHROUGH

		// "zl" - scroll screen to the left
    case 'l':
    case K_RIGHT:
		if (!curwin->w_p_wrap)
		{
		    // scroll the window left
		    curwin->w_leftcol += (colnr_T)cap->count1;
		    leftcol_changed();
		}
		break;

		// "zs" - scroll screen, cursor at the start
    case 's':	if (!curwin->w_p_wrap)
		{
#ifdef FEAT_FOLDING
		    if (hasFolding(curwin->w_cursor.lnum, NULL, NULL))
			col = 0;	// like the cursor is in col 0
		    else
#endif
		    getvcol(curwin, &curwin->w_cursor, &col, NULL, NULL);
		    if ((long)col > siso)
			col -= siso;
		    else
			col = 0;
		    if (curwin->w_leftcol != col)
		    {
			curwin->w_leftcol = col;
			redraw_later(NOT_VALID);
		    }
		}
		break;

		// "ze" - scroll screen, cursor at the end
    case 'e':	if (!curwin->w_p_wrap)
		{
#ifdef FEAT_FOLDING
		    if (hasFolding(curwin->w_cursor.lnum, NULL, NULL))
			col = 0;	// like the cursor is in col 0
		    else
#endif
		    getvcol(curwin, &curwin->w_cursor, NULL, NULL, &col);
		    n = curwin->w_width - curwin_col_off();
		    if ((long)col + siso < n)
			col = 0;
		    else
			col = col + siso - n + 1;
		    if (curwin->w_leftcol != col)
		    {
			curwin->w_leftcol = col;
			redraw_later(NOT_VALID);
		    }
		}
		break;

		// "zp", "zP" in block mode put without addind trailing spaces
    case 'P':
    case 'p':  nv_put(cap);
	       break;
		// "zy" Yank without trailing spaces
    case 'y':  nv_operator(cap);
	       break;
#ifdef FEAT_FOLDING
		// "zF": create fold command
		// "zf": create fold operator
    case 'F':
    case 'f':   if (foldManualAllowed(TRUE))
		{
		    cap->nchar = 'f';
		    nv_operator(cap);
		    curwin->w_p_fen = TRUE;

		    // "zF" is like "zfzf"
		    if (nchar == 'F' && cap->oap->op_type == OP_FOLD)
		    {
			nv_operator(cap);
			finish_op = TRUE;
		    }
		}
		else
		    clearopbeep(cap->oap);
		break;

		// "zd": delete fold at cursor
		// "zD": delete fold at cursor recursively
    case 'd':
    case 'D':	if (foldManualAllowed(FALSE))
		{
		    if (VIsual_active)
			nv_operator(cap);
		    else
			deleteFold(curwin->w_cursor.lnum,
				  curwin->w_cursor.lnum, nchar == 'D', FALSE);
		}
		break;

		// "zE": erase all folds
    case 'E':	if (foldmethodIsManual(curwin))
		{
		    clearFolding(curwin);
		    changed_window_setting();
		}
		else if (foldmethodIsMarker(curwin))
		    deleteFold((linenr_T)1, curbuf->b_ml.ml_line_count,
								 TRUE, FALSE);
		else
		    emsg(_("E352: Cannot erase folds with current 'foldmethod'"));
		break;

		// "zn": fold none: reset 'foldenable'
    case 'n':	curwin->w_p_fen = FALSE;
		break;

		// "zN": fold Normal: set 'foldenable'
    case 'N':	curwin->w_p_fen = TRUE;
		break;

		// "zi": invert folding: toggle 'foldenable'
    case 'i':	curwin->w_p_fen = !curwin->w_p_fen;
		break;

		// "za": open closed fold or close open fold at cursor
    case 'a':	if (hasFolding(curwin->w_cursor.lnum, NULL, NULL))
		    openFold(curwin->w_cursor.lnum, cap->count1);
		else
		{
		    closeFold(curwin->w_cursor.lnum, cap->count1);
		    curwin->w_p_fen = TRUE;
		}
		break;

		// "zA": open fold at cursor recursively
    case 'A':	if (hasFolding(curwin->w_cursor.lnum, NULL, NULL))
		    openFoldRecurse(curwin->w_cursor.lnum);
		else
		{
		    closeFoldRecurse(curwin->w_cursor.lnum);
		    curwin->w_p_fen = TRUE;
		}
		break;

		// "zo": open fold at cursor or Visual area
    case 'o':	if (VIsual_active)
		    nv_operator(cap);
		else
		    openFold(curwin->w_cursor.lnum, cap->count1);
		break;

		// "zO": open fold recursively
    case 'O':	if (VIsual_active)
		    nv_operator(cap);
		else
		    openFoldRecurse(curwin->w_cursor.lnum);
		break;

		// "zc": close fold at cursor or Visual area
    case 'c':	if (VIsual_active)
		    nv_operator(cap);
		else
		    closeFold(curwin->w_cursor.lnum, cap->count1);
		curwin->w_p_fen = TRUE;
		break;

		// "zC": close fold recursively
    case 'C':	if (VIsual_active)
		    nv_operator(cap);
		else
		    closeFoldRecurse(curwin->w_cursor.lnum);
		curwin->w_p_fen = TRUE;
		break;

		// "zv": open folds at the cursor
    case 'v':	foldOpenCursor();
		break;

		// "zx": re-apply 'foldlevel' and open folds at the cursor
    case 'x':	curwin->w_p_fen = TRUE;
		curwin->w_foldinvalid = TRUE;	// recompute folds
		newFoldLevel();			// update right now
		foldOpenCursor();
		break;

		// "zX": undo manual opens/closes, re-apply 'foldlevel'
    case 'X':	curwin->w_p_fen = TRUE;
		curwin->w_foldinvalid = TRUE;	// recompute folds
		old_fdl = -1;			// force an update
		break;

		// "zm": fold more
    case 'm':	if (curwin->w_p_fdl > 0)
		{
		    curwin->w_p_fdl -= cap->count1;
		    if (curwin->w_p_fdl < 0)
			curwin->w_p_fdl = 0;
		}
		old_fdl = -1;		// force an update
		curwin->w_p_fen = TRUE;
		break;

		// "zM": close all folds
    case 'M':	curwin->w_p_fdl = 0;
		old_fdl = -1;		// force an update
		curwin->w_p_fen = TRUE;
		break;

		// "zr": reduce folding
    case 'r':	curwin->w_p_fdl += cap->count1;
		{
		    int d = getDeepestNesting();

		    if (curwin->w_p_fdl >= d)
			curwin->w_p_fdl = d;
		}
		break;

		// "zR": open all folds
    case 'R':	curwin->w_p_fdl = getDeepestNesting();
		old_fdl = -1;		// force an update
		break;

    case 'j':	// "zj" move to next fold downwards
    case 'k':	// "zk" move to next fold upwards
		if (foldMoveTo(TRUE, nchar == 'j' ? FORWARD : BACKWARD,
							  cap->count1) == FAIL)
		    clearopbeep(cap->oap);
		break;

#endif // FEAT_FOLDING

#ifdef FEAT_SPELL
    case 'u':	// "zug" and "zuw": undo "zg" and "zw"
		++no_mapping;
		++allow_keys;   // no mapping for nchar, but allow key codes
		nchar = plain_vgetc();
		LANGMAP_ADJUST(nchar, TRUE);
		--no_mapping;
		--allow_keys;
#ifdef FEAT_CMDL_INFO
		(void)add_to_showcmd(nchar);
#endif
		if (vim_strchr((char_u *)"gGwW", nchar) == NULL)
		{
		    clearopbeep(cap->oap);
		    break;
		}
		undo = TRUE;
		// FALLTHROUGH

    case 'g':	// "zg": add good word to word list
    case 'w':	// "zw": add wrong word to word list
    case 'G':	// "zG": add good word to temp word list
    case 'W':	// "zW": add wrong word to temp word list
		{
		    char_u  *ptr = NULL;
		    int	    len;

		    if (checkclearop(cap->oap))
			break;
		    if (VIsual_active && get_visual_text(cap, &ptr, &len)
								      == FAIL)
			return;
		    if (ptr == NULL)
		    {
			pos_T	pos = curwin->w_cursor;

			// Find bad word under the cursor.  When 'spell' is
			// off this fails and find_ident_under_cursor() is
			// used below.
			emsg_off++;
			len = spell_move_to(curwin, FORWARD, TRUE, TRUE, NULL);
			emsg_off--;
			if (len != 0 && curwin->w_cursor.col <= pos.col)
			    ptr = ml_get_pos(&curwin->w_cursor);
			curwin->w_cursor = pos;
		    }

		    if (ptr == NULL && (len = find_ident_under_cursor(&ptr,
							    FIND_IDENT)) == 0)
			return;
		    spell_add_word(ptr, len, nchar == 'w' || nchar == 'W'
					      ? SPELL_ADD_BAD : SPELL_ADD_GOOD,
					    (nchar == 'G' || nchar == 'W')
						       ? 0 : (int)cap->count1,
					    undo);
		}
		break;

    case '=':	// "z=": suggestions for a badly spelled word
		if (!checkclearop(cap->oap))
		    spell_suggest((int)cap->count0);
		break;
#endif

    default:	clearopbeep(cap->oap);
    }

#ifdef FEAT_FOLDING
    // Redraw when 'foldenable' changed
    if (old_fen != curwin->w_p_fen)
    {
# ifdef FEAT_DIFF
	win_T	    *wp;

	if (foldmethodIsDiff(curwin) && curwin->w_p_scb)
	{
	    // Adjust 'foldenable' in diff-synced windows.
	    FOR_ALL_WINDOWS(wp)
	    {
		if (wp != curwin && foldmethodIsDiff(wp) && wp->w_p_scb)
		{
		    wp->w_p_fen = curwin->w_p_fen;
		    changed_window_setting_win(wp);
		}
	    }
	}
# endif
	changed_window_setting();
    }

    // Redraw when 'foldlevel' changed.
    if (old_fdl != curwin->w_p_fdl)
	newFoldLevel();
#endif
}