inet_sockaddrtopair(struct sockaddr_storage *addr)
{
	char addr_str[INET6_ADDRSTRLEN];
	static char ret[sizeof(addr_str) + 8];	/* '[' + addr_str + ']' + ':' + 'nnnnn' */

	inet_sockaddrtos2(addr, addr_str);
	snprintf(ret, sizeof(ret), "[%s]:%d"
		, addr_str
		, ntohs(inet_sockaddrport(addr)));
	return ret;
}