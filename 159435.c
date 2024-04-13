n_opencmd(cmdarg_T *cap)
{
#ifdef FEAT_CONCEAL
    linenr_T	oldline = curwin->w_cursor.lnum;
#endif

    if (!checkclearopq(cap->oap))
    {
#ifdef FEAT_FOLDING
	if (cap->cmdchar == 'O')
	    // Open above the first line of a folded sequence of lines
	    (void)hasFolding(curwin->w_cursor.lnum,
						&curwin->w_cursor.lnum, NULL);
	else
	    // Open below the last line of a folded sequence of lines
	    (void)hasFolding(curwin->w_cursor.lnum,
						NULL, &curwin->w_cursor.lnum);
#endif
	if (u_save((linenr_T)(curwin->w_cursor.lnum -
					       (cap->cmdchar == 'O' ? 1 : 0)),
		   (linenr_T)(curwin->w_cursor.lnum +
					       (cap->cmdchar == 'o' ? 1 : 0))
		       ) == OK
		&& open_line(cap->cmdchar == 'O' ? BACKWARD : FORWARD,
			 has_format_option(FO_OPEN_COMS) ? OPENLINE_DO_COM : 0,
								      0) == OK)
	{
#ifdef FEAT_CONCEAL
	    if (curwin->w_p_cole > 0 && oldline != curwin->w_cursor.lnum)
		redrawWinline(curwin, oldline);
#endif
#ifdef FEAT_SYN_HL
	    if (curwin->w_p_cul)
		// force redraw of cursorline
		curwin->w_valid &= ~VALID_CROW;
#endif
	    // When '#' is in 'cpoptions' ignore the count.
	    if (vim_strchr(p_cpo, CPO_HASH) != NULL)
		cap->count1 = 1;
	    invoke_edit(cap, FALSE, cap->cmdchar, TRUE);
	}
    }
}