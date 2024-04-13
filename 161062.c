ins_reg(void)
{
    int		need_redraw = FALSE;
    int		regname;
    int		literally = 0;
    int		vis_active = VIsual_active;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_status = PC_STATUS_UNSET;
    if (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('"', TRUE);
#ifdef FEAT_CMDL_INFO
	add_to_showcmd_c(Ctrl_R);
#endif
    }

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    /*
     * Don't map the register name. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_keys;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    if (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
#ifdef FEAT_CMDL_INFO
	add_to_showcmd_c(literally);
#endif
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_keys;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    if (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_INPUT_METHOD
	int	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_expr_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_INPUT_METHOD
	// Restore the Input Method.
	if (im_on)
	    im_set_active(TRUE);
# endif
    }
    if (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    else
    {
#endif
	if (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	else if (insert_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	else if (stop_insert_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    if (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
#ifdef FEAT_CMDL_INFO
    clear_showcmd();
#endif

    // If the inserted register is empty, we need to remove the '"'
    if (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    if (!vis_active && VIsual_active)
	end_visual_mode();
}