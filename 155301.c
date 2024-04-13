static void cmm_timer_fn(struct timer_list *unused)
{
	long nr;

	nr = cmm_timed_pages_target - cmm_timeout_pages;
	if (nr < 0)
		cmm_timed_pages_target = 0;
	else
		cmm_timed_pages_target = nr;
	cmm_kick_thread();
	cmm_set_timer();
}