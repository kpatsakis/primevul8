sighup_handler(int sig)
{
	/* Linux has "one-shot" signals, reinstall the signal handler */
	signal(SIGHUP, sighup_handler);

	dlog(LOG_DEBUG, 4, "sighup_handler called");

	sighup_received = 1;
}