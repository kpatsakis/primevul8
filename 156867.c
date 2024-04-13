inet_stosockaddr(char *ip, const char *port, struct sockaddr_storage *addr)
{
	void *addr_ip;
	char *cp;
	char sav_cp;
	unsigned port_num;
	int res;

	addr->ss_family = (strchr(ip, ':')) ? AF_INET6 : AF_INET;

	if (port) {
		if (!read_unsigned(port, &port_num, 1, 65535, true)) {
			addr->ss_family = AF_UNSPEC;
			return -1;
		}
	}

	if (addr->ss_family == AF_INET6) {
		struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) addr;
		if (port)
			addr6->sin6_port = htons(port_num);
		addr_ip = &addr6->sin6_addr;
	} else {
		struct sockaddr_in *addr4 = (struct sockaddr_in *) addr;
		if (port)
			addr4->sin_port = htons(port_num);
		addr_ip = &addr4->sin_addr;
	}

	/* remove range and mask stuff */
	if ((cp = strchr(ip, '-')) ||
	    (cp = strchr(ip, '/'))) {
		sav_cp = *cp;
		*cp = 0;
	}

	res = inet_pton(addr->ss_family, ip, addr_ip);

	/* restore range and mask stuff */
	if (cp)
		*cp = sav_cp;

	if (!res) {
		addr->ss_family = AF_UNSPEC;
		return -1;
	}

	return 0;
}