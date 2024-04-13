inet_sockaddrip6(struct sockaddr_storage *addr, struct in6_addr *ip6)
{
	if (addr->ss_family != AF_INET6)
		return -1;

	*ip6 = ((struct sockaddr_in6 *) addr)->sin6_addr;
	return 0;
}