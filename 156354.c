pthread_join(sp_pthread_t thread, void **value __unused)
{
	int status;

	waitpid(thread, &status, 0);
	return (status);
}