win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    if (after == NULL)	    /* after NULL is in front of the first */
	before = firstwin;
    else
	before = after->w_next;

    wp->w_next = before;
    wp->w_prev = after;
    if (after == NULL)
	firstwin = wp;
    else
	after->w_next = wp;
    if (before == NULL)
	lastwin = wp;
    else
	before->w_prev = wp;
}