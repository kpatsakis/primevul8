static void cmm_kick_thread(void)
{
	wake_up(&cmm_thread_wait);
}