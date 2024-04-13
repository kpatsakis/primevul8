make_windows(
    int		count,
    int		vertical UNUSED)  /* split windows vertically if TRUE */
{
    int		maxcount;
    int		todo;

    if (vertical)
    {
	/* Each windows needs at least 'winminwidth' lines and a separator
	 * column. */
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    else
    {
	/* Each window needs at least 'winminheight' lines and a status line. */
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
				  - (p_wh - p_wmh)) / (p_wmh + STATUS_HEIGHT);
    }

    if (maxcount < 2)
	maxcount = 2;
    if (count > maxcount)
	count = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    if (count > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    /* todo is number of windows left to create */
    for (todo = count - 1; todo > 0; --todo)
	if (vertical)
	{
	    if (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		break;
	}
	else
	{
	    if (win_split(curwin->w_height - (curwin->w_height - todo
			    * STATUS_HEIGHT) / (todo + 1)
			- STATUS_HEIGHT, WSP_ABOVE) == FAIL)
		break;
	}

    unblock_autocmds();

    /* return actual number of windows */
    return (count - todo);
}