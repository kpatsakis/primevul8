version(void)
{
	fprintf(stderr, "Version: %s\n\n", VERSION);
	fprintf(stderr, "Compiled in settings:\n");
	fprintf(stderr, "  default config file		\"%s\"\n", PATH_RADVD_CONF);
	fprintf(stderr, "  default pidfile		\"%s\"\n", PATH_RADVD_PID);
	fprintf(stderr, "  default logfile		\"%s\"\n", PATH_RADVD_LOG);
	fprintf(stderr, "  default syslog facility	%d\n", LOG_FACILITY);
	fprintf(stderr, "Please send bug reports or suggestions to %s.\n",
		CONTACT_EMAIL);

	exit(1);
}