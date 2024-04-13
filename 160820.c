static int kvp_process_ip_address(void *addrp,
				int family, char *buffer,
				int length,  int *offset)
{
	struct sockaddr_in *addr;
	struct sockaddr_in6 *addr6;
	int addr_length;
	char tmp[50];
	const char *str;

	if (family == AF_INET) {
		addr = (struct sockaddr_in *)addrp;
		str = inet_ntop(family, &addr->sin_addr, tmp, 50);
		addr_length = INET_ADDRSTRLEN;
	} else {
		addr6 = (struct sockaddr_in6 *)addrp;
		str = inet_ntop(family, &addr6->sin6_addr.s6_addr, tmp, 50);
		addr_length = INET6_ADDRSTRLEN;
	}

	if ((length - *offset) < addr_length + 2)
		return HV_E_FAIL;
	if (str == NULL) {
		strcpy(buffer, "inet_ntop failed\n");
		return HV_E_FAIL;
	}
	if (*offset == 0)
		strcpy(buffer, tmp);
	else {
		strcat(buffer, ";");
		strcat(buffer, tmp);
	}

	*offset += strlen(str) + 1;

	return 0;
}