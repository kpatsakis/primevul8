nv_right(cmdarg_T *cap)
{
    long	n;
    int		past_line;

    if (mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL))
    {
	// <C-Right> and <S-Right> move a word or WORD right
	if (mod_mask & MOD_MASK_CTRL)
	    cap->arg = TRUE;
	nv_wordcmd(cap);
	return;
    }

    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    past_line = (VIsual_active && *p_sel != 'o');

    /*
     * In virtual edit mode, there's no such thing as "past_line", as lines
     * are (theoretically) infinitely long.
     */
    if (virtual_active())
	past_line = 0;

    for (n = cap->count1; n > 0; --n)
    {
	if ((!past_line && oneright() == FAIL)
		|| (past_line && *ml_get_cursor() == NUL)
		)
	{
	    /*
	     *	  <Space> wraps to next line if 'whichwrap' has 's'.
	     *	      'l' wraps to next line if 'whichwrap' has 'l'.
	     * CURS_RIGHT wraps to next line if 'whichwrap' has '>'.
	     */
	    if (       ((cap->cmdchar == ' '
			    && vim_strchr(p_ww, 's') != NULL)
			|| (cap->cmdchar == 'l'
			    && vim_strchr(p_ww, 'l') != NULL)
			|| (cap->cmdchar == K_RIGHT
			    && vim_strchr(p_ww, '>') != NULL))
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	    {
		// When deleting we also count the NL as a character.
		// Set cap->oap->inclusive when last char in the line is
		// included, move to next line after that
		if (	   cap->oap->op_type != OP_NOP
			&& !cap->oap->inclusive
			&& !LINEEMPTY(curwin->w_cursor.lnum))
		    cap->oap->inclusive = TRUE;
		else
		{
		    ++curwin->w_cursor.lnum;
		    curwin->w_cursor.col = 0;
		    curwin->w_cursor.coladd = 0;
		    curwin->w_set_curswant = TRUE;
		    cap->oap->inclusive = FALSE;
		}
		continue;
	    }
	    if (cap->oap->op_type == OP_NOP)
	    {
		// Only beep and flush if not moved at all
		if (n == cap->count1)
		    beep_flush();
	    }
	    else
	    {
		if (!LINEEMPTY(curwin->w_cursor.lnum))
		    cap->oap->inclusive = TRUE;
	    }
	    break;
	}
	else if (past_line)
	{
	    curwin->w_set_curswant = TRUE;
	    if (virtual_active())
		oneright();
	    else
	    {
		if (has_mbyte)
		    curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
		else
		    ++curwin->w_cursor.col;
	    }
	}
    }
#ifdef FEAT_FOLDING
    if (n != cap->count1 && (fdo_flags & FDO_HOR) && KeyTyped
					       && cap->oap->op_type == OP_NOP)
	foldOpenCursor();
#endif
}