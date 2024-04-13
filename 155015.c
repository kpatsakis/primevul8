win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(NULL, FALSE);
    if (oldwin == NULL)
    {
	/* Very first window, need to create an empty buffer for it and
	 * initialize from scratch. */
	curbuf = buflist_new(NULL, NULL, 1L, BLN_LISTED);
	if (curwin == NULL || curbuf == NULL)
	    return FAIL;
	curwin->w_buffer = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	/* there is one window */
	curwin->w_alist = &global_alist;
	curwin_init();		/* init current window */
    }
    else
    {
	/* First window in new tab page, initialize it from "oldwin". */
	win_init(curwin, oldwin, 0);

	/* We don't want cursor- and scroll-binding in the first window. */
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    if (curwin->w_frame == NULL)
	return FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = Columns;
    topframe->fr_height = Rows - p_ch;

    return OK;
}