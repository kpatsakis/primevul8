alt_tabpage(void)
{
    tabpage_T	*tp;

    /* Use the next tab page if possible. */
    if (curtab->tp_next != NULL)
	return curtab->tp_next;

    /* Find the last but one tab page. */
    for (tp = first_tabpage; tp->tp_next != curtab; tp = tp->tp_next)
	;
    return tp;
}