ins_esc(
    long	*count,
    int		cmdchar,
    int		nomove)	    // don't move cursor
{
    int		temp;
    static int	disabled_redraw = FALSE;

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    temp = curwin->w_cursor.col;
    if (disabled_redraw)
    {
	--RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    if (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	if (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	if (*count > 0)
	{
	    line_breakcheck();
	    if (got_int)
		*count = 0;
	}

	if (--*count > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    if (vim_strchr(p_cpo, CPO_REPLCNT) != NULL)
		State &= ~REPLACE_FLAG;

	    (void)start_redo_ins();
	    if (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    return FALSE;	// repeat the insert
	}
	stop_insert(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    // When an autoindent was removed, curswant stays after the
    // indent
    if (restart_edit == NUL && (colnr_T)temp == curwin->w_cursor.col)
	curwin->w_set_curswant = TRUE;

    // Remember the last Insert position in the '^ mark.
    if (!cmdmod.keepjumps)
	curbuf->b_last_insert = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    if (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	if (curwin->w_cursor.coladd > 0 || ve_flags == VE_ALL)
	{
	    oneleft();
	    if (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	else
	{
	    --curwin->w_cursor.col;
	    // Correct cursor for multi-byte character.
	    if (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_INPUT_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    if (!(State & LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = NORMAL;
    // need to position cursor again (e.g. when on a TAB )
    changed_cline_bef_curs();

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    if (!p_ek)
    {
	// Re-enable bracketed paste mode.
	out_str(T_BE);

	// Re-enable modifyOtherKeys.
	out_str(T_CTI);
    }

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    if (reg_recording != 0 || restart_edit != NUL)
	showmode();
    else if (p_smd && (got_int || !skip_showmode()))
	msg("");

    return TRUE;	    // exit Insert mode
}