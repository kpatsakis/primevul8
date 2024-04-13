pthread_create(sp_pthread_t *thread, const void *attr __unused,
    void *(*thread_start)(void *), void *arg)
{
	pid_t pid;

	switch ((pid = fork())) {
	case -1:
		error("fork(): %s", strerror(errno));
		return (-1);
	case 0:
		thread_start(arg);
		_exit(1);
	default:
		*thread = pid;
		return (0);
	}
}