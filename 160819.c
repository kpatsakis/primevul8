static int is_ipv4(char *addr)
{
	int ret;
	struct in_addr ipv4_addr;

	ret = inet_pton(AF_INET, addr, &ipv4_addr);

	if (ret == 1)
		return 1;
	return 0;
}