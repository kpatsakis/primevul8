static int cmm_oom_notify(struct notifier_block *self,
			  unsigned long dummy, void *parm)
{
	unsigned long *freed = parm;
	long nr = 256;

	nr = cmm_free_pages(nr, &cmm_timed_pages, &cmm_timed_page_list);
	if (nr > 0)
		nr = cmm_free_pages(nr, &cmm_pages, &cmm_page_list);
	cmm_pages_target = cmm_pages;
	cmm_timed_pages_target = cmm_timed_pages;
	*freed += 256 - nr;
	return NOTIFY_OK;
}