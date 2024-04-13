static void cmm_set_timer(void)
{
	if (cmm_timed_pages_target <= 0 || cmm_timeout_seconds <= 0) {
		if (timer_pending(&cmm_timer))
			del_timer(&cmm_timer);
		return;
	}
	mod_timer(&cmm_timer, jiffies + cmm_timeout_seconds * HZ);
}