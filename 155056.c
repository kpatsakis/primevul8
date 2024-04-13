win_horz_neighbor(tabpage_T *tp, win_T *wp, int left, long count)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

    foundfr = wp->w_frame;
    while (count--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	for (;;)
	{
	    if (fr == tp->tp_topframe)
		goto end;
	    if (left)
		nfr = fr->fr_prev;
	    else
		nfr = fr->fr_next;
	    if (fr->fr_parent->fr_layout == FR_ROW && nfr != NULL)
		break;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	for (;;)
	{
	    if (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		break;
	    }
	    fr = nfr->fr_child;
	    if (nfr->fr_layout == FR_COL)
	    {
		/* Find the frame at the cursor row. */
		while (fr->fr_next != NULL
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_next;
	    }
	    if (nfr->fr_layout == FR_ROW && left)
		while (fr->fr_next != NULL)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    return foundfr != NULL ? foundfr->fr_win : NULL;
}