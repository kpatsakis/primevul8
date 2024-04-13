win_totop(int size, int flags)
{
    int		dir;
    int		height = curwin->w_height;

    if (ONE_WINDOW)
    {
	beep_flush();
	return;
    }
    if (check_split_disallowed() == FAIL)
	return;

    /* Remove the window and frame from the tree of frames. */
    (void)winframe_remove(curwin, &dir, NULL);
    win_remove(curwin, NULL);
    last_status(FALSE);	    /* may need to remove last status line */
    (void)win_comp_pos();   /* recompute window positions */

    /* Split a window on the desired side and put the window there. */
    (void)win_split_ins(size, flags, curwin, dir);
    if (!(flags & WSP_VERT))
    {
	win_setheight(height);
	if (p_ea)
	    win_equal(curwin, TRUE, 'v');
    }

#if defined(FEAT_GUI)
    /* When 'guioptions' includes 'L' or 'R' may have to remove or add
     * scrollbars.  Have to update them anyway. */
    gui_may_update_scrollbars();
#endif
}