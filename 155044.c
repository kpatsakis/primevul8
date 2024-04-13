frame_fixed_height(frame_T *frp)
{
    /* frame with one window: fixed height if 'winfixheight' set. */
    if (frp->fr_win != NULL)
	return frp->fr_win->w_p_wfh;

    if (frp->fr_layout == FR_ROW)
    {
	/* The frame is fixed height if one of the frames in the row is fixed
	 * height. */
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    if (frame_fixed_height(frp))
		return TRUE;
	return FALSE;
    }

    /* frp->fr_layout == FR_COL: The frame is fixed height if all of the
     * frames in the row are fixed height. */
    FOR_ALL_FRAMES(frp, frp->fr_child)
	if (!frame_fixed_height(frp))
	    return FALSE;
    return TRUE;
}