comp_textwidth(
    int		ff)	// force formatting (for "gq" command)
{
    int		textwidth;

    textwidth = curbuf->b_p_tw;
    if (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
#ifdef FEAT_CMDWIN
	if (cmdwin_type != 0)
	    textwidth -= 1;
#endif
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	if (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	if (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    if (textwidth < 0)
	textwidth = 0;
    if (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	if (textwidth > 79)
	    textwidth = 79;
    }
    return textwidth;
}