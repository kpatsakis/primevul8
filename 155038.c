win_rotate(int upwards, int count)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    int		n;

    if (ONE_WINDOW)		/* nothing to do */
    {
	beep_flush();
	return;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /* Check if all frames in this row/col have one window. */
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	if (frp->fr_win == NULL)
	{
	    emsg(_("E443: Cannot rotate when another window is split"));
	    return;
	}

    while (count--)
    {
	if (upwards)		/* first window becomes last window */
	{
	    /* remove first window/frame from the list */
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, NULL);
	    frame_remove(frp);

	    /* find last frame and append removed window/frame after it */
	    for ( ; frp->fr_next != NULL; frp = frp->fr_next)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		/* previously last window */
	}
	else			/* last window becomes first window */
	{
	    /* find last window/frame in the list and remove it */
	    for (frp = curwin->w_frame; frp->fr_next != NULL;
							   frp = frp->fr_next)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    /* will become last window */
	    win_remove(wp1, NULL);
	    frame_remove(frp);

	    /* append the removed window/frame before the first in the list */
	    win_append(frp->fr_parent->fr_child->fr_win->w_prev, wp1);
	    frame_insert(frp->fr_parent->fr_child, frp);
	}

	/* exchange status height and vsep width of old and new last window */
	n = wp2->w_status_height;
	wp2->w_status_height = wp1->w_status_height;
	wp1->w_status_height = n;
	frame_fix_height(wp1);
	frame_fix_height(wp2);
	n = wp2->w_vsep_width;
	wp2->w_vsep_width = wp1->w_vsep_width;
	wp1->w_vsep_width = n;
	frame_fix_width(wp1);
	frame_fix_width(wp2);

	/* recompute w_winrow and w_wincol for all windows */
	(void)win_comp_pos();
    }

    redraw_all_later(NOT_VALID);
}