win_free_mem(
    win_T	*win,
    int		*dirp,		/* set to 'v' or 'h' for direction if 'ea' */
    tabpage_T	*tp)		/* tab page "win" is in, NULL for current */
{
    frame_T	*frp;
    win_T	*wp;

    /* Remove the window and its frame from the tree of frames. */
    frp = win->w_frame;
    wp = winframe_remove(win, dirp, tp);
    vim_free(frp);
    win_free(win, tp);

    /* When deleting the current window of another tab page select a new
     * current window. */
    if (tp != NULL && win == tp->tp_curwin)
	tp->tp_curwin = wp;

    return wp;
}