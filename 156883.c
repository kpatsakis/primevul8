inet_stor(const char *addr, uint32_t *range_end)
{
	const char *cp;
	char *endptr;
	unsigned long range;
	int family = strchr(addr, ':') ? AF_INET6 : AF_INET;
	char *warn = "";

#ifndef _STRICT_CONFIG_
	if (!__test_bit(CONFIG_TEST_BIT, &debug))
		warn = "WARNING - ";
#endif

	/* Return UINT32_MAX to indicate no range */
	if (!(cp = strchr(addr, '-'))) {
		*range_end = UINT32_MAX;
		return true;
	}

	errno = 0;
	range = strtoul(cp + 1, &endptr, family == AF_INET6 ? 16 : 10);
	*range_end = range;

	if (*endptr)
		report_config_error(CONFIG_INVALID_NUMBER, "%sVirtual server group range '%s' has extra characters at end '%s'", warn, addr, endptr);
	else if (errno == ERANGE ||
		 (family == AF_INET6 && range > 0xffff) ||
		 (family == AF_INET && range > 255)) {
		report_config_error(CONFIG_INVALID_NUMBER, "Virtual server group range '%s' end '%s' too large", addr, cp + 1);

		/* Indicate error */
		return false;
	}
	else
		return true;

#ifdef _STRICT_CONFIG_
        return false;
#else
        return !__test_bit(CONFIG_TEST_BIT, &debug);
#endif
}