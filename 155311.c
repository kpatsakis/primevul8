static void cmm_add_timed_pages(long nr)
{
	cmm_timed_pages_target += nr;
	cmm_kick_thread();
}