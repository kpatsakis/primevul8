nv_wordcmd(cmdarg_T *cap)
{
    int		n;
    int		word_end;
    int		flag = FALSE;
    pos_T	startpos = curwin->w_cursor;

    /*
     * Set inclusive for the "E" and "e" command.
     */
    if (cap->cmdchar == 'e' || cap->cmdchar == 'E')
	word_end = TRUE;
    else
	word_end = FALSE;
    cap->oap->inclusive = word_end;

    /*
     * "cw" and "cW" are a special case.
     */
    if (!word_end && cap->oap->op_type == OP_CHANGE)
    {
	n = gchar_cursor();
	if (n != NUL)			// not an empty line
	{
	    if (VIM_ISWHITE(n))
	    {
		/*
		 * Reproduce a funny Vi behaviour: "cw" on a blank only
		 * changes one character, not all blanks until the start of
		 * the next word.  Only do this when the 'w' flag is included
		 * in 'cpoptions'.
		 */
		if (cap->count1 == 1 && vim_strchr(p_cpo, CPO_CW) != NULL)
		{
		    cap->oap->inclusive = TRUE;
		    cap->oap->motion_type = MCHAR;
		    return;
		}
	    }
	    else
	    {
		/*
		 * This is a little strange. To match what the real Vi does,
		 * we effectively map 'cw' to 'ce', and 'cW' to 'cE', provided
		 * that we are not on a space or a TAB.  This seems impolite
		 * at first, but it's really more what we mean when we say
		 * 'cw'.
		 * Another strangeness: When standing on the end of a word
		 * "ce" will change until the end of the next word, but "cw"
		 * will change only one character! This is done by setting
		 * flag.
		 */
		cap->oap->inclusive = TRUE;
		word_end = TRUE;
		flag = TRUE;
	    }
	}
    }

    cap->oap->motion_type = MCHAR;
    curwin->w_set_curswant = TRUE;
    if (word_end)
	n = end_word(cap->count1, cap->arg, flag, FALSE);
    else
	n = fwd_word(cap->count1, cap->arg, cap->oap->op_type != OP_NOP);

    // Don't leave the cursor on the NUL past the end of line. Unless we
    // didn't move it forward.
    if (LT_POS(startpos, curwin->w_cursor))
	adjust_cursor(cap->oap);

    if (n == FAIL && cap->oap->op_type == OP_NOP)
	clearopbeep(cap->oap);
    else
    {
	adjust_for_sel(cap);
#ifdef FEAT_FOLDING
	if ((fdo_flags & FDO_HOR) && KeyTyped && cap->oap->op_type == OP_NOP)
	    foldOpenCursor();
#endif
    }
}