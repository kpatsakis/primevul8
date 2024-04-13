frame_fixed_width(frame_T *frp)
{
    /* frame with one window: fixed width if 'winfixwidth' set. */
    if (frp->fr_win != NULL)
	return frp->fr_win->w_p_wfw;

    if (frp->fr_layout == FR_COL)
    {
	/* The frame is fixed width if one of the frames in the row is fixed
	 * width. */
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    if (frame_fixed_width(frp))
		return TRUE;
	return FALSE;
    }

    /* frp->fr_layout == FR_ROW: The frame is fixed width if all of the
     * frames in the row are fixed width. */
    FOR_ALL_FRAMES(frp, frp->fr_child)
	if (!frame_fixed_width(frp))
	    return FALSE;
    return TRUE;
}