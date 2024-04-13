inet_sockaddrtos(struct sockaddr_storage *addr)
{
	static char addr_str[INET6_ADDRSTRLEN];
	inet_sockaddrtos2(addr, addr_str);
	return addr_str;
}