domain_stosockaddr(const char *domain, const char *port, struct sockaddr_storage *addr)
{
	struct addrinfo *res = NULL;
	unsigned port_num;

	if (port) {
		if (!read_unsigned(port, &port_num, 1, 65535, true)) {
			addr->ss_family = AF_UNSPEC;
			return -1;
		}
	}

	if (getaddrinfo(domain, NULL, NULL, &res) != 0 || !res) {
		addr->ss_family = AF_UNSPEC;
		return -1;
	}

	addr->ss_family = (sa_family_t)res->ai_family;

	if (addr->ss_family == AF_INET6) {
		struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
		*addr6 = *(struct sockaddr_in6 *)res->ai_addr;
		if (port)
			addr6->sin6_port = htons(port_num);
	} else {
		struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
		*addr4 = *(struct sockaddr_in *)res->ai_addr;
		if (port)
			addr4->sin_port = htons(port_num);
	}

	freeaddrinfo(res);

	return 0;
}