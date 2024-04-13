adjust_cursor_eol(void)
{
    unsigned int cur_ve_flags = get_ve_flags();

    if (curwin->w_cursor.col > 0
	    && gchar_cursor() == NUL
	    && (cur_ve_flags & VE_ONEMORE) == 0
	    && !(restart_edit || (State & MODE_INSERT)))
    {
	// Put the cursor on the last character in the line.
	dec_cursor();

	if (cur_ve_flags == VE_ALL)
	{
	    colnr_T	    scol, ecol;

	    // Coladd is set to the width of the last character.
	    getvcol(curwin, &curwin->w_cursor, &scol, NULL, &ecol);
	    curwin->w_cursor.coladd = ecol - scol + 1;
	}
    }
}