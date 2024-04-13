check_ip6_forwarding(void)
{
	int forw_sysctl[] = { SYSCTL_IP6_FORWARDING };
	int value;
	size_t size = sizeof(value);
	FILE *fp = NULL;
	static int warned = 0;

#ifdef __linux__
	fp = fopen(PROC_SYS_IP6_FORWARDING, "r");
	if (fp) {
		int rc = fscanf(fp, "%d", &value);
		if(rc != 1){
			flog(LOG_ERR, "cannot read value from %s: %s", PROC_SYS_IP6_FORWARDING, strerror(errno));
			exit(1);
		}
		fclose(fp);
	}
	else
		flog(LOG_DEBUG, "Correct IPv6 forwarding procfs entry not found, "
	                       "perhaps the procfs is disabled, "
	                        "or the kernel interface has changed?");
#endif /* __linux__ */

	if (!fp && sysctl(forw_sysctl, sizeof(forw_sysctl)/sizeof(forw_sysctl[0]),
	    &value, &size, NULL, 0) < 0) {
		flog(LOG_DEBUG, "Correct IPv6 forwarding sysctl branch not found, "
			"perhaps the kernel interface has changed?");
		return(0);	/* this is of advisory value only */
	}

	if (value != 1 && !warned) {
		warned = 1;
		flog(LOG_DEBUG, "IPv6 forwarding setting is: %u, should be 1", value);
		return(-1);
	}

	return(0);
}