make_tabpages(int maxcount)
{
    int		count = maxcount;
    int		todo;

    /* Limit to 'tabpagemax' tabs. */
    if (count > p_tpm)
	count = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    for (todo = count - 1; todo > 0; --todo)
	if (win_new_tabpage(0) == FAIL)
	    break;

    unblock_autocmds();

    /* return actual number of tab pages */
    return (count - todo);
}