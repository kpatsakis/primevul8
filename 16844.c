ins_pageup(void)
{
    pos_T	tpos;

    undisplay_dollar();

    if (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	if (first_tabpage->tp_next != NULL)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	return;
    }

    tpos = curwin->w_cursor;
    if (onepage(BACKWARD, 1L) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    else
	vim_beep(BO_CRSR);
}