frame_comp_pos(frame_T *topfrp, int *row, int *col)
{
    win_T	*wp;
    frame_T	*frp;
    int		startcol;
    int		startrow;
    int		h;

    wp = topfrp->fr_win;
    if (wp != NULL)
    {
	if (wp->w_winrow != *row || wp->w_wincol != *col)
	{
	    /* position changed, redraw */
	    wp->w_winrow = *row;
	    wp->w_wincol = *col;
	    redraw_win_later(wp, NOT_VALID);
	    wp->w_redr_status = TRUE;
	}
	/* WinBar will not show if the window height is zero */
	h = VISIBLE_HEIGHT(wp) + wp->w_status_height;
	*row += h > topfrp->fr_height ? topfrp->fr_height : h;
	*col += wp->w_width + wp->w_vsep_width;
    }
    else
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    if (topfrp->fr_layout == FR_ROW)
		*row = startrow;	/* all frames are at the same row */
	    else
		*col = startcol;	/* all frames are at the same col */
	    frame_comp_pos(frp, row, col);
	}
    }
}