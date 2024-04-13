inet_ip6tosockaddr(struct in6_addr *sin_addr, struct sockaddr_storage *addr)
{
	struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) addr;
	addr6->sin6_family = AF_INET6;
	addr6->sin6_addr = *sin_addr;
}