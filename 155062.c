restore_snapshot(
    int		idx,
    int		close_curwin)	    /* closing current window */
{
    win_T	*wp;

    if (curtab->tp_snapshot[idx] != NULL
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	if (wp != NULL && close_curwin)
	    win_goto(wp);
	redraw_all_later(NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}