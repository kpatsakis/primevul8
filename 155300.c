static void cmm_set_timeout(long nr, long seconds)
{
	cmm_timeout_pages = nr;
	cmm_timeout_seconds = seconds;
	cmm_set_timer();
}