win_free_all(void)
{
    int		dummy;

    while (first_tabpage->tp_next != NULL)
	tabpage_close(TRUE);

    if (aucmd_win != NULL)
    {
	(void)win_free_mem(aucmd_win, &dummy, NULL);
	aucmd_win = NULL;
    }
# ifdef FEAT_TEXT_PROP
    close_all_popups();
# endif

    while (firstwin != NULL)
	(void)win_free_mem(firstwin, &dummy, NULL);

    /* No window should be used after this. Set curwin to NULL to crash
     * instead of using freed memory. */
    curwin = NULL;
}