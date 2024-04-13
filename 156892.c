inet_sockaddrip4(struct sockaddr_storage *addr)
{
	if (addr->ss_family != AF_INET)
		return 0xffffffff;

	return ((struct sockaddr_in *) addr)->sin_addr.s_addr;
}