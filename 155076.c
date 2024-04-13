tabpage_move(int nr)
{
    int		n = 1;
    tabpage_T	*tp, *tp_dst;

    if (first_tabpage->tp_next == NULL)
	return;

    for (tp = first_tabpage; tp->tp_next != NULL && n < nr; tp = tp->tp_next)
	++n;

    if (tp == curtab || (nr > 0 && tp->tp_next != NULL
						    && tp->tp_next == curtab))
	return;

    tp_dst = tp;

    /* Remove the current tab page from the list of tab pages. */
    if (curtab == first_tabpage)
	first_tabpage = curtab->tp_next;
    else
    {
	FOR_ALL_TABPAGES(tp)
	    if (tp->tp_next == curtab)
		break;
	if (tp == NULL)	/* "cannot happen" */
	    return;
	tp->tp_next = curtab->tp_next;
    }

    /* Re-insert it at the specified position. */
    if (nr <= 0)
    {
	curtab->tp_next = first_tabpage;
	first_tabpage = curtab;
    }
    else
    {
	curtab->tp_next = tp_dst->tp_next;
	tp_dst->tp_next = curtab;
    }

    /* Need to redraw the tabline.  Tab page contents doesn't change. */
    redraw_tabline = TRUE;
}