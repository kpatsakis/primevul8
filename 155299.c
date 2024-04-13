static void cmm_set_pages(long nr)
{
	cmm_pages_target = nr;
	cmm_kick_thread();
}