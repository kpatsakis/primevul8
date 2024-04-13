inet_sockaddrport(struct sockaddr_storage *addr)
{
	uint16_t port;

	if (addr->ss_family == AF_INET6) {
		struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) addr;
		port = addr6->sin6_port;
	} else {
		/* Note: this might be AF_UNSPEC if it is the sequence number of
		 * a virtual server in a virtual server group */
		struct sockaddr_in *addr4 = (struct sockaddr_in *) addr;
		port = addr4->sin_port;
	}

	return port;
}