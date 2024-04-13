ins_horscroll(void)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    if (gui_do_horiz_scroll(scrollbar_value, FALSE))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}