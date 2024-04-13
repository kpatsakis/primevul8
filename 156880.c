inet_inaddrcmp(const int family, const void *a, const void *b)
{
	int64_t addr_diff;

	if (family == AF_INET) {
		addr_diff = (int64_t)ntohl(*((const uint32_t *) a)) - (int64_t)ntohl(*((const uint32_t *) b));
		if (addr_diff > 0)
			return 1;
		if (addr_diff < 0)
			return -1;
		return 0;
	}

	if (family == AF_INET6) {
		int i;

		for (i = 0; i < 4; i++ ) {
			addr_diff = (int64_t)ntohl(((const uint32_t *) (a))[i]) - (int64_t)ntohl(((const uint32_t *) (b))[i]);
			if (addr_diff > 0)
				return 1;
			if (addr_diff < 0)
				return -1;
		}
		return 0;
	}

	return -2;
}