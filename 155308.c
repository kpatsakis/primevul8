static int cmm_resume(void)
{
	cmm_suspended = 0;
	cmm_kick_thread();
	return 0;
}