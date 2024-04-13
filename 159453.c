nv_brackets(cmdarg_T *cap)
{
    pos_T	new_pos = {0, 0, 0};
    pos_T	prev_pos;
    pos_T	*pos = NULL;	    // init for GCC
    pos_T	old_pos;	    // cursor position before command
    int		flag;
    long	n;
    int		findc;
    int		c;

    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    old_pos = curwin->w_cursor;
    curwin->w_cursor.coladd = 0;    // TODO: don't do this for an error.

#ifdef FEAT_SEARCHPATH
    /*
     * "[f" or "]f" : Edit file under the cursor (same as "gf")
     */
    if (cap->nchar == 'f')
	nv_gotofile(cap);
    else
#endif

#ifdef FEAT_FIND_ID
    /*
     * Find the occurrence(s) of the identifier or define under cursor
     * in current and included files or jump to the first occurrence.
     *
     *			search	     list	    jump
     *		      fwd   bwd    fwd	 bwd	 fwd	bwd
     * identifier     "]i"  "[i"   "]I"  "[I"	"]^I"  "[^I"
     * define	      "]d"  "[d"   "]D"  "[D"	"]^D"  "[^D"
     */
    if (vim_strchr((char_u *)
# ifdef EBCDIC
		"iI\005dD\067",
# else
		"iI\011dD\004",
# endif
		cap->nchar) != NULL)
    {
	char_u	*ptr;
	int	len;

	if ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
	    clearop(cap->oap);
	else
	{
	    find_pattern_in_path(ptr, 0, len, TRUE,
		cap->count0 == 0 ? !isupper(cap->nchar) : FALSE,
		((cap->nchar & 0xf) == ('d' & 0xf)) ?  FIND_DEFINE : FIND_ANY,
		cap->count1,
		isupper(cap->nchar) ? ACTION_SHOW_ALL :
			    islower(cap->nchar) ? ACTION_SHOW : ACTION_GOTO,
		cap->cmdchar == ']' ? curwin->w_cursor.lnum + 1 : (linenr_T)1,
		(linenr_T)MAXLNUM);
	    curwin->w_set_curswant = TRUE;
	}
    }
    else
#endif

    /*
     * "[{", "[(", "]}" or "])": go to Nth unclosed '{', '(', '}' or ')'
     * "[#", "]#": go to start/end of Nth innermost #if..#endif construct.
     * "[/", "[*", "]/", "]*": go to Nth comment start/end.
     * "[m" or "]m" search for prev/next start of (Java) method.
     * "[M" or "]M" search for prev/next end of (Java) method.
     */
    if (  (cap->cmdchar == '['
		&& vim_strchr((char_u *)"{(*/#mM", cap->nchar) != NULL)
	    || (cap->cmdchar == ']'
		&& vim_strchr((char_u *)"})*/#mM", cap->nchar) != NULL))
    {
	if (cap->nchar == '*')
	    cap->nchar = '/';
	prev_pos.lnum = 0;
	if (cap->nchar == 'm' || cap->nchar == 'M')
	{
	    if (cap->cmdchar == '[')
		findc = '{';
	    else
		findc = '}';
	    n = 9999;
	}
	else
	{
	    findc = cap->nchar;
	    n = cap->count1;
	}
	for ( ; n > 0; --n)
	{
	    if ((pos = findmatchlimit(cap->oap, findc,
		(cap->cmdchar == '[') ? FM_BACKWARD : FM_FORWARD, 0)) == NULL)
	    {
		if (new_pos.lnum == 0)	// nothing found
		{
		    if (cap->nchar != 'm' && cap->nchar != 'M')
			clearopbeep(cap->oap);
		}
		else
		    pos = &new_pos;	// use last one found
		break;
	    }
	    prev_pos = new_pos;
	    curwin->w_cursor = *pos;
	    new_pos = *pos;
	}
	curwin->w_cursor = old_pos;

	/*
	 * Handle "[m", "]m", "[M" and "[M".  The findmatchlimit() only
	 * brought us to the match for "[m" and "]M" when inside a method.
	 * Try finding the '{' or '}' we want to be at.
	 * Also repeat for the given count.
	 */
	if (cap->nchar == 'm' || cap->nchar == 'M')
	{
	    // norm is TRUE for "]M" and "[m"
	    int	    norm = ((findc == '{') == (cap->nchar == 'm'));

	    n = cap->count1;
	    // found a match: we were inside a method
	    if (prev_pos.lnum != 0)
	    {
		pos = &prev_pos;
		curwin->w_cursor = prev_pos;
		if (norm)
		    --n;
	    }
	    else
		pos = NULL;
	    while (n > 0)
	    {
		for (;;)
		{
		    if ((findc == '{' ? dec_cursor() : inc_cursor()) < 0)
		    {
			// if not found anything, that's an error
			if (pos == NULL)
			    clearopbeep(cap->oap);
			n = 0;
			break;
		    }
		    c = gchar_cursor();
		    if (c == '{' || c == '}')
		    {
			// Must have found end/start of class: use it.
			// Or found the place to be at.
			if ((c == findc && norm) || (n == 1 && !norm))
			{
			    new_pos = curwin->w_cursor;
			    pos = &new_pos;
			    n = 0;
			}
			// if no match found at all, we started outside of the
			// class and we're inside now.  Just go on.
			else if (new_pos.lnum == 0)
			{
			    new_pos = curwin->w_cursor;
			    pos = &new_pos;
			}
			// found start/end of other method: go to match
			else if ((pos = findmatchlimit(cap->oap, findc,
			    (cap->cmdchar == '[') ? FM_BACKWARD : FM_FORWARD,
								  0)) == NULL)
			    n = 0;
			else
			    curwin->w_cursor = *pos;
			break;
		    }
		}
		--n;
	    }
	    curwin->w_cursor = old_pos;
	    if (pos == NULL && new_pos.lnum != 0)
		clearopbeep(cap->oap);
	}
	if (pos != NULL)
	{
	    setpcmark();
	    curwin->w_cursor = *pos;
	    curwin->w_set_curswant = TRUE;
#ifdef FEAT_FOLDING
	    if ((fdo_flags & FDO_BLOCK) && KeyTyped
					       && cap->oap->op_type == OP_NOP)
		foldOpenCursor();
#endif
	}
    }

    /*
     * "[[", "[]", "]]" and "][": move to start or end of function
     */
    else if (cap->nchar == '[' || cap->nchar == ']')
    {
	if (cap->nchar == cap->cmdchar)		    // "]]" or "[["
	    flag = '{';
	else
	    flag = '}';		    // "][" or "[]"

	curwin->w_set_curswant = TRUE;
	/*
	 * Imitate strange Vi behaviour: When using "]]" with an operator
	 * we also stop at '}'.
	 */
	if (!findpar(&cap->oap->inclusive, cap->arg, cap->count1, flag,
	      (cap->oap->op_type != OP_NOP
				      && cap->arg == FORWARD && flag == '{')))
	    clearopbeep(cap->oap);
	else
	{
	    if (cap->oap->op_type == OP_NOP)
		beginline(BL_WHITE | BL_FIX);
#ifdef FEAT_FOLDING
	    if ((fdo_flags & FDO_BLOCK) && KeyTyped && cap->oap->op_type == OP_NOP)
		foldOpenCursor();
#endif
	}
    }

    /*
     * "[p", "[P", "]P" and "]p": put with indent adjustment
     */
    else if (cap->nchar == 'p' || cap->nchar == 'P')
    {
	nv_put_opt(cap, TRUE);
    }

    /*
     * "['", "[`", "]'" and "]`": jump to next mark
     */
    else if (cap->nchar == '\'' || cap->nchar == '`')
    {
	pos = &curwin->w_cursor;
	for (n = cap->count1; n > 0; --n)
	{
	    prev_pos = *pos;
	    pos = getnextmark(pos, cap->cmdchar == '[' ? BACKWARD : FORWARD,
							  cap->nchar == '\'');
	    if (pos == NULL)
		break;
	}
	if (pos == NULL)
	    pos = &prev_pos;
	nv_cursormark(cap, cap->nchar == '\'', pos);
    }

    /*
     * [ or ] followed by a middle mouse click: put selected text with
     * indent adjustment.  Any other button just does as usual.
     */
    else if (cap->nchar >= K_RIGHTRELEASE && cap->nchar <= K_LEFTMOUSE)
    {
	(void)do_mouse(cap->oap, cap->nchar,
		       (cap->cmdchar == ']') ? FORWARD : BACKWARD,
		       cap->count1, PUT_FIXINDENT);
    }

#ifdef FEAT_FOLDING
    /*
     * "[z" and "]z": move to start or end of open fold.
     */
    else if (cap->nchar == 'z')
    {
	if (foldMoveTo(FALSE, cap->cmdchar == ']' ? FORWARD : BACKWARD,
							 cap->count1) == FAIL)
	    clearopbeep(cap->oap);
    }
#endif

#ifdef FEAT_DIFF
    /*
     * "[c" and "]c": move to next or previous diff-change.
     */
    else if (cap->nchar == 'c')
    {
	if (diff_move_to(cap->cmdchar == ']' ? FORWARD : BACKWARD,
							 cap->count1) == FAIL)
	    clearopbeep(cap->oap);
    }
#endif

#ifdef FEAT_SPELL
    /*
     * "[s", "[S", "]s" and "]S": move to next spell error.
     */
    else if (cap->nchar == 's' || cap->nchar == 'S')
    {
	setpcmark();
	for (n = 0; n < cap->count1; ++n)
	    if (spell_move_to(curwin, cap->cmdchar == ']' ? FORWARD : BACKWARD,
			  cap->nchar == 's' ? TRUE : FALSE, FALSE, NULL) == 0)
	    {
		clearopbeep(cap->oap);
		break;
	    }
	    else
		curwin->w_set_curswant = TRUE;
# ifdef FEAT_FOLDING
	if (cap->oap->op_type == OP_NOP && (fdo_flags & FDO_SEARCH) && KeyTyped)
	    foldOpenCursor();
# endif
    }
#endif

    // Not a valid cap->nchar.
    else
	clearopbeep(cap->oap);
}