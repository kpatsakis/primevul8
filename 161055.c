ins_digraph(void)
{
    int	    c;
    int	    cc;
    int	    did_putchar = FALSE;

    pc_status = PC_STATUS_UNSET;
    if (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('?', TRUE);
	did_putchar = TRUE;
#ifdef FEAT_CMDL_INFO
	add_to_showcmd_c(Ctrl_K);
#endif
    }

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // don't map the digraph chars. This also prevents the
    // mode message to be deleted when ESC is hit
    ++no_mapping;
    ++allow_keys;
    c = plain_vgetc();
    --no_mapping;
    --allow_keys;
    if (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    if (IS_SPECIAL(c) || mod_mask)	    // special key
    {
#ifdef FEAT_CMDL_INFO
	clear_showcmd();
#endif
	insert_special(c, TRUE, FALSE);
	return NUL;
    }
    if (c != ESC)
    {
	did_putchar = FALSE;
	if (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    if (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
#ifdef FEAT_CMDL_INFO
	    add_to_showcmd_c(c);
#endif
	}
	++no_mapping;
	++allow_keys;
	cc = plain_vgetc();
	--no_mapping;
	--allow_keys;
	if (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	if (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = getdigraph(c, cc, TRUE);
#ifdef FEAT_CMDL_INFO
	    clear_showcmd();
#endif
	    return c;
	}
    }
#ifdef FEAT_CMDL_INFO
    clear_showcmd();
#endif
    return NUL;
}