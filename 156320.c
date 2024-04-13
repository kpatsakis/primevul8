pthread_cancel(sp_pthread_t thread)
{
	return (kill(thread, SIGTERM));
}