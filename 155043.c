win_setminwidth(void)
{
    int		room;
    int		needed;
    int		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    while (p_wmw > 0)
    {
	room = Columns;
	needed = frame_minwidth(topframe, NULL);
	if (room >= needed)
	    break;
	--p_wmw;
	if (first)
	{
	    emsg(_(e_noroom));
	    first = FALSE;
	}
    }
}