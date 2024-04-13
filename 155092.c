win_setminheight(void)
{
    int		room;
    int		needed;
    int		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    while (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = frame_minheight(topframe, NULL);
	if (room >= needed)
	    break;
	--p_wmh;
	if (first)
	{
	    emsg(_(e_noroom));
	    first = FALSE;
	}
    }
}