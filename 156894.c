inet_ip4tosockaddr(struct in_addr *sin_addr, struct sockaddr_storage *addr)
{
	struct sockaddr_in *addr4 = (struct sockaddr_in *) addr;
	addr4->sin_family = AF_INET;
	addr4->sin_addr = *sin_addr;
}