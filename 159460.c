nv_g_cmd(cmdarg_T *cap)
{
    oparg_T	*oap = cap->oap;
    pos_T	tpos;
    int		i;
    int		flag = FALSE;

    switch (cap->nchar)
    {
    case Ctrl_A:
    case Ctrl_X:
#ifdef MEM_PROFILE
    /*
     * "g^A": dump log of used memory.
     */
	if (!VIsual_active && cap->nchar == Ctrl_A)
	    vim_mem_profile_dump();
	else
#endif
    /*
     * "g^A/g^X": sequentially increment visually selected region
     */
	     if (VIsual_active)
	{
	    cap->arg = TRUE;
	    cap->cmdchar = cap->nchar;
	    cap->nchar = NUL;
	    nv_addsub(cap);
	}
	else
	    clearopbeep(oap);
	break;

    /*
     * "gR": Enter virtual replace mode.
     */
    case 'R':
	cap->arg = TRUE;
	nv_Replace(cap);
	break;

    case 'r':
	nv_vreplace(cap);
	break;

    case '&':
	do_cmdline_cmd((char_u *)"%s//~/&");
	break;

    /*
     * "gv": Reselect the previous Visual area.  If Visual already active,
     *	     exchange previous and current Visual area.
     */
    case 'v':
	if (checkclearop(oap))
	    break;

	if (	   curbuf->b_visual.vi_start.lnum == 0
		|| curbuf->b_visual.vi_start.lnum > curbuf->b_ml.ml_line_count
		|| curbuf->b_visual.vi_end.lnum == 0)
	    beep_flush();
	else
	{
	    // set w_cursor to the start of the Visual area, tpos to the end
	    if (VIsual_active)
	    {
		i = VIsual_mode;
		VIsual_mode = curbuf->b_visual.vi_mode;
		curbuf->b_visual.vi_mode = i;
# ifdef FEAT_EVAL
		curbuf->b_visual_mode_eval = i;
# endif
		i = curwin->w_curswant;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
		curbuf->b_visual.vi_curswant = i;

		tpos = curbuf->b_visual.vi_end;
		curbuf->b_visual.vi_end = curwin->w_cursor;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curbuf->b_visual.vi_start = VIsual;
	    }
	    else
	    {
		VIsual_mode = curbuf->b_visual.vi_mode;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
	    }

	    VIsual_active = TRUE;
	    VIsual_reselect = TRUE;

	    // Set Visual to the start and w_cursor to the end of the Visual
	    // area.  Make sure they are on an existing character.
	    check_cursor();
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = tpos;
	    check_cursor();
	    update_topline();
	    /*
	     * When called from normal "g" command: start Select mode when
	     * 'selectmode' contains "cmd".  When called for K_SELECT, always
	     * start Select mode.
	     */
	    if (cap->arg)
		VIsual_select = TRUE;
	    else
		may_start_select('c');
	    setmouse();
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end are still the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	    redraw_curbuf_later(INVERTED);
	    showmode();
	}
	break;
    /*
     * "gV": Don't reselect the previous Visual area after a Select mode
     *	     mapping of menu.
     */
    case 'V':
	VIsual_reselect = FALSE;
	break;

    /*
     * "gh":  start Select mode.
     * "gH":  start Select line mode.
     * "g^H": start Select block mode.
     */
    case K_BS:
	cap->nchar = Ctrl_H;
	// FALLTHROUGH
    case 'h':
    case 'H':
    case Ctrl_H:
# ifdef EBCDIC
	// EBCDIC: 'v'-'h' != '^v'-'^h'
	if (cap->nchar == Ctrl_H)
	    cap->cmdchar = Ctrl_V;
	else
# endif
	cap->cmdchar = cap->nchar + ('v' - 'h');
	cap->arg = TRUE;
	nv_visual(cap);
	break;

    // "gn", "gN" visually select next/previous search match
    // "gn" selects next match
    // "gN" selects previous match
    case 'N':
    case 'n':
	if (!current_search(cap->count1, cap->nchar == 'n'))
	    clearopbeep(oap);
	break;

    /*
     * "gj" and "gk" two new funny movement keys -- up and down
     * movement based on *screen* line rather than *file* line.
     */
    case 'j':
    case K_DOWN:
	// with 'nowrap' it works just like the normal "j" command.
	if (!curwin->w_p_wrap)
	{
	    oap->motion_type = MLINE;
	    i = cursor_down(cap->count1, oap->op_type == OP_NOP);
	}
	else
	    i = nv_screengo(oap, FORWARD, cap->count1);
	if (i == FAIL)
	    clearopbeep(oap);
	break;

    case 'k':
    case K_UP:
	// with 'nowrap' it works just like the normal "k" command.
	if (!curwin->w_p_wrap)
	{
	    oap->motion_type = MLINE;
	    i = cursor_up(cap->count1, oap->op_type == OP_NOP);
	}
	else
	    i = nv_screengo(oap, BACKWARD, cap->count1);
	if (i == FAIL)
	    clearopbeep(oap);
	break;

    /*
     * "gJ": join two lines without inserting a space.
     */
    case 'J':
	nv_join(cap);
	break;

    /*
     * "g0", "g^" and "g$": Like "0", "^" and "$" but for screen lines.
     * "gm": middle of "g0" and "g$".
     */
    case '^':
	flag = TRUE;
	// FALLTHROUGH

    case '0':
    case 'm':
    case K_HOME:
    case K_KHOME:
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	if (curwin->w_p_wrap && curwin->w_width != 0)
	{
	    int		width1 = curwin->w_width - curwin_col_off();
	    int		width2 = width1 + curwin_col_off2();

	    validate_virtcol();
	    i = 0;
	    if (curwin->w_virtcol >= (colnr_T)width1 && width2 > 0)
		i = (curwin->w_virtcol - width1) / width2 * width2 + width1;
	}
	else
	    i = curwin->w_leftcol;
	// Go to the middle of the screen line.  When 'number' or
	// 'relativenumber' is on and lines are wrapping the middle can be more
	// to the left.
	if (cap->nchar == 'm')
	    i += (curwin->w_width - curwin_col_off()
		    + ((curwin->w_p_wrap && i > 0)
			? curwin_col_off2() : 0)) / 2;
	coladvance((colnr_T)i);
	if (flag)
	{
	    do
		i = gchar_cursor();
	    while (VIM_ISWHITE(i) && oneright() == OK);
	    curwin->w_valid &= ~VALID_WCOL;
	}
	curwin->w_set_curswant = TRUE;
	break;

    case 'M':
	{
	    char_u  *ptr = ml_get_curline();

	    oap->motion_type = MCHAR;
	    oap->inclusive = FALSE;
	    if (has_mbyte)
		i = mb_string2cells(ptr, (int)STRLEN(ptr));
	    else
		i = (int)STRLEN(ptr);
	    if (cap->count0 > 0 && cap->count0 <= 100)
		coladvance((colnr_T)(i * cap->count0 / 100));
	    else
		coladvance((colnr_T)(i / 2));
	    curwin->w_set_curswant = TRUE;
	}
	break;

    case '_':
	// "g_": to the last non-blank character in the line or <count> lines
	// downward.
	cap->oap->motion_type = MCHAR;
	cap->oap->inclusive = TRUE;
	curwin->w_curswant = MAXCOL;
	if (cursor_down((long)(cap->count1 - 1),
					 cap->oap->op_type == OP_NOP) == FAIL)
	    clearopbeep(cap->oap);
	else
	{
	    char_u  *ptr = ml_get_curline();

	    // In Visual mode we may end up after the line.
	    if (curwin->w_cursor.col > 0 && ptr[curwin->w_cursor.col] == NUL)
		--curwin->w_cursor.col;

	    // Decrease the cursor column until it's on a non-blank.
	    while (curwin->w_cursor.col > 0
				    && VIM_ISWHITE(ptr[curwin->w_cursor.col]))
		--curwin->w_cursor.col;
	    curwin->w_set_curswant = TRUE;
	    adjust_for_sel(cap);
	}
	break;

    case '$':
    case K_END:
    case K_KEND:
	{
	    int col_off = curwin_col_off();

	    oap->motion_type = MCHAR;
	    oap->inclusive = TRUE;
	    if (curwin->w_p_wrap && curwin->w_width != 0)
	    {
		curwin->w_curswant = MAXCOL;    // so we stay at the end
		if (cap->count1 == 1)
		{
		    int		width1 = curwin->w_width - col_off;
		    int		width2 = width1 + curwin_col_off2();

		    validate_virtcol();
		    i = width1 - 1;
		    if (curwin->w_virtcol >= (colnr_T)width1)
			i += ((curwin->w_virtcol - width1) / width2 + 1)
								     * width2;
		    coladvance((colnr_T)i);

		    // Make sure we stick in this column.
		    validate_virtcol();
		    curwin->w_curswant = curwin->w_virtcol;
		    curwin->w_set_curswant = FALSE;
		    if (curwin->w_cursor.col > 0 && curwin->w_p_wrap)
		    {
			/*
			 * Check for landing on a character that got split at
			 * the end of the line.  We do not want to advance to
			 * the next screen line.
			 */
			if (curwin->w_virtcol > (colnr_T)i)
			    --curwin->w_cursor.col;
		    }
		}
		else if (nv_screengo(oap, FORWARD, cap->count1 - 1) == FAIL)
		    clearopbeep(oap);
	    }
	    else
	    {
		if (cap->count1 > 1)
		    // if it fails, let the cursor still move to the last char
		    (void)cursor_down(cap->count1 - 1, FALSE);

		i = curwin->w_leftcol + curwin->w_width - col_off - 1;
		coladvance((colnr_T)i);

		// if the character doesn't fit move one back
		if (curwin->w_cursor.col > 0
				       && (*mb_ptr2cells)(ml_get_cursor()) > 1)
		{
		    colnr_T vcol;

		    getvvcol(curwin, &curwin->w_cursor, NULL, NULL, &vcol);
		    if (vcol >= curwin->w_leftcol + curwin->w_width - col_off)
			--curwin->w_cursor.col;
		}

		// Make sure we stick in this column.
		validate_virtcol();
		curwin->w_curswant = curwin->w_virtcol;
		curwin->w_set_curswant = FALSE;
	    }
	}
	break;

    /*
     * "g*" and "g#", like "*" and "#" but without using "\<" and "\>"
     */
    case '*':
    case '#':
#if POUND != '#'
    case POUND:		// pound sign (sometimes equal to '#')
#endif
    case Ctrl_RSB:		// :tag or :tselect for current identifier
    case ']':			// :tselect for current identifier
	nv_ident(cap);
	break;

    /*
     * ge and gE: go back to end of word
     */
    case 'e':
    case 'E':
	oap->motion_type = MCHAR;
	curwin->w_set_curswant = TRUE;
	oap->inclusive = TRUE;
	if (bckend_word(cap->count1, cap->nchar == 'E', FALSE) == FAIL)
	    clearopbeep(oap);
	break;

    /*
     * "g CTRL-G": display info about cursor position
     */
    case Ctrl_G:
	cursor_pos_info(NULL);
	break;

    /*
     * "gi": start Insert at the last position.
     */
    case 'i':
	if (curbuf->b_last_insert.lnum != 0)
	{
	    curwin->w_cursor = curbuf->b_last_insert;
	    check_cursor_lnum();
	    i = (int)STRLEN(ml_get_curline());
	    if (curwin->w_cursor.col > (colnr_T)i)
	    {
		if (virtual_active())
		    curwin->w_cursor.coladd += curwin->w_cursor.col - i;
		curwin->w_cursor.col = i;
	    }
	}
	cap->cmdchar = 'i';
	nv_edit(cap);
	break;

    /*
     * "gI": Start insert in column 1.
     */
    case 'I':
	beginline(0);
	if (!checkclearopq(oap))
	    invoke_edit(cap, FALSE, 'g', FALSE);
	break;

#ifdef FEAT_SEARCHPATH
    /*
     * "gf": goto file, edit file under cursor
     * "]f" and "[f": can also be used.
     */
    case 'f':
    case 'F':
	nv_gotofile(cap);
	break;
#endif

	// "g'm" and "g`m": jump to mark without setting pcmark
    case '\'':
	cap->arg = TRUE;
	// FALLTHROUGH
    case '`':
	nv_gomark(cap);
	break;

    /*
     * "gs": Goto sleep.
     */
    case 's':
	do_sleep(cap->count1 * 1000L, FALSE);
	break;

    /*
     * "ga": Display the ascii value of the character under the
     * cursor.	It is displayed in decimal, hex, and octal. -- webb
     */
    case 'a':
	do_ascii(NULL);
	break;

    /*
     * "g8": Display the bytes used for the UTF-8 character under the
     * cursor.	It is displayed in hex.
     * "8g8" finds illegal byte sequence.
     */
    case '8':
	if (cap->count0 == 8)
	    utf_find_illegal();
	else
	    show_utf8();
	break;

    // "g<": show scrollback text
    case '<':
	show_sb_text();
	break;

    /*
     * "gg": Goto the first line in file.  With a count it goes to
     * that line number like for "G". -- webb
     */
    case 'g':
	cap->arg = FALSE;
	nv_goto(cap);
	break;

    /*
     *	 Two-character operators:
     *	 "gq"	    Format text
     *	 "gw"	    Format text and keep cursor position
     *	 "g~"	    Toggle the case of the text.
     *	 "gu"	    Change text to lower case.
     *	 "gU"	    Change text to upper case.
     *   "g?"	    rot13 encoding
     *   "g@"	    call 'operatorfunc'
     */
    case 'q':
    case 'w':
	oap->cursor_start = curwin->w_cursor;
	// FALLTHROUGH
    case '~':
    case 'u':
    case 'U':
    case '?':
    case '@':
	nv_operator(cap);
	break;

    /*
     * "gd": Find first occurrence of pattern under the cursor in the
     *	 current function
     * "gD": idem, but in the current file.
     */
    case 'd':
    case 'D':
	nv_gd(oap, cap->nchar, (int)cap->count0);
	break;

    /*
     * g<*Mouse> : <C-*mouse>
     */
    case K_MIDDLEMOUSE:
    case K_MIDDLEDRAG:
    case K_MIDDLERELEASE:
    case K_LEFTMOUSE:
    case K_LEFTDRAG:
    case K_LEFTRELEASE:
    case K_MOUSEMOVE:
    case K_RIGHTMOUSE:
    case K_RIGHTDRAG:
    case K_RIGHTRELEASE:
    case K_X1MOUSE:
    case K_X1DRAG:
    case K_X1RELEASE:
    case K_X2MOUSE:
    case K_X2DRAG:
    case K_X2RELEASE:
	mod_mask = MOD_MASK_CTRL;
	(void)do_mouse(oap, cap->nchar, BACKWARD, cap->count1, 0);
	break;

    case K_IGNORE:
	break;

    /*
     * "gP" and "gp": same as "P" and "p" but leave cursor just after new text
     */
    case 'p':
    case 'P':
	nv_put(cap);
	break;

#ifdef FEAT_BYTEOFF
    // "go": goto byte count from start of buffer
    case 'o':
	goto_byte(cap->count0);
	break;
#endif

    // "gQ": improved Ex mode
    case 'Q':
	if (text_locked())
	{
	    clearopbeep(cap->oap);
	    text_locked_msg();
	    break;
	}

	if (!checkclearopq(oap))
	    do_exmode(TRUE);
	break;

#ifdef FEAT_JUMPLIST
    case ',':
	nv_pcmark(cap);
	break;

    case ';':
	cap->count1 = -cap->count1;
	nv_pcmark(cap);
	break;
#endif

    case 't':
	if (!checkclearop(oap))
	    goto_tabpage((int)cap->count0);
	break;
    case 'T':
	if (!checkclearop(oap))
	    goto_tabpage(-(int)cap->count1);
	break;

    case TAB:
	if (!checkclearop(oap) && goto_tabpage_lastused() == FAIL)
	    clearopbeep(oap);
	break;

    case '+':
    case '-': // "g+" and "g-": undo or redo along the timeline
	if (!checkclearopq(oap))
	    undo_time(cap->nchar == '-' ? -cap->count1 : cap->count1,
							 FALSE, FALSE, FALSE);
	break;

    default:
	clearopbeep(oap);
	break;
    }
}