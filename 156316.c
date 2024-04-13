monitor_socketpair(int *pair)
{
#ifdef HAVE_SOCKETPAIR
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1)
		fatal("%s: socketpair", __func__);
#else
	fatal("%s: UsePrivilegeSeparation=yes not supported",
	    __func__);
#endif
	FD_CLOSEONEXEC(pair[0]);
	FD_CLOSEONEXEC(pair[1]);
}