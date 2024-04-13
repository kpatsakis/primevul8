static int cmm_suspend(void)
{
	cmm_suspended = 1;
	cmm_free_pages(cmm_pages, &cmm_pages, &cmm_page_list);
	cmm_free_pages(cmm_timed_pages, &cmm_timed_pages, &cmm_timed_page_list);
	return 0;
}