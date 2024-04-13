ins_scroll(void)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    if (gui_do_scroll())
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}