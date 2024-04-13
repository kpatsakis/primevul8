win_remove(
    win_T	*wp,
    tabpage_T	*tp)		/* tab page "win" is in, NULL for current */
{
    if (wp->w_prev != NULL)
	wp->w_prev->w_next = wp->w_next;
    else if (tp == NULL)
	firstwin = curtab->tp_firstwin = wp->w_next;
    else
	tp->tp_firstwin = wp->w_next;

    if (wp->w_next != NULL)
	wp->w_next->w_prev = wp->w_prev;
    else if (tp == NULL)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    else
	tp->tp_lastwin = wp->w_prev;
}