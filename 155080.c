win_altframe(
    win_T	*win,
    tabpage_T	*tp)		/* tab page "win" is in, NULL for current */
{
    frame_T	*frp;
    frame_T	*other_fr, *target_fr;

    if (tp == NULL ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	return alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    if (frp->fr_prev == NULL)
	return frp->fr_next;
    if (frp->fr_next == NULL)
	return frp->fr_prev;

    target_fr = frp->fr_next;
    other_fr  = frp->fr_prev;
    if (p_spr || p_sb)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    /* If 'wfh' or 'wfw' is set for the target and not for the alternate
     * window, reverse the selection. */
    if (frp->fr_parent != NULL && frp->fr_parent->fr_layout == FR_ROW)
    {
	if (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    else
    {
	if (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    return target_fr;
}