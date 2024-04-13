ins_pagedown(void)
{
    pos_T	tpos;

    undisplay_dollar();

    if (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	if (first_tabpage->tp_next != NULL)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	return;
    }

    tpos = curwin->w_cursor;
    if (onepage(FORWARD, 1L) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    else
	vim_beep(BO_CRSR);
}