min_rows(void)
{
    int		total;
    tabpage_T	*tp;
    int		n;

    if (firstwin == NULL)	/* not initialized yet */
	return MIN_LINES;

    total = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, NULL);
	if (total < n)
	    total = n;
    }
    total += tabline_height();
    total += 1;		/* count the room for the command line */
    return total;
}