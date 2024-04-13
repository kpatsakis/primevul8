ins_ctrl_v(void)
{
    int		c;
    int		did_putchar = FALSE;
    int		prev_mod_mask = mod_mask;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    if (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

#ifdef FEAT_CMDL_INFO
    add_to_showcmd_c(Ctrl_V);
#endif

    c = get_literal();
    if (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
#ifdef FEAT_CMDL_INFO
    clear_showcmd();
#endif

    if ((c == ESC || c == CSI) && !(prev_mod_mask & MOD_MASK_SHIFT))
	// Using CTRL-V: Change any modifyOtherKeys ESC sequence to a normal
	// key.  Don't do this for CTRL-SHIFT-V.
	c = decodeModifyOtherKeys(c);

    insert_special(c, FALSE, TRUE);
#ifdef FEAT_RIGHTLEFT
    revins_chars++;
    revins_legal++;
#endif
}