win_alloc_first(void)
{
    if (win_alloc_firstwin(NULL) == FAIL)
	return FAIL;

    first_tabpage = alloc_tabpage();
    if (first_tabpage == NULL)
	return FAIL;
    first_tabpage->tp_topframe = topframe;
    curtab = first_tabpage;

    return OK;
}