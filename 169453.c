sigterm_handler(int sig)
{
	/* Linux has "one-shot" signals, reinstall the signal handler */
	signal(SIGTERM, sigterm_handler);

	dlog(LOG_DEBUG, 4, "sigterm_handler called");

	++sigterm_received;

	if(sigterm_received > 1){
		dlog(LOG_ERR, 1, "sigterm_handler called %d times...aborting...", sigterm_received);
		abort();
	}
}