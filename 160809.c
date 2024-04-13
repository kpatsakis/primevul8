kvp_get_domain_name(char *buffer, int length)
{
	struct addrinfo	hints, *info ;
	int error = 0;

	gethostname(buffer, length);
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; /*Get only ipv4 addrinfo. */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	error = getaddrinfo(buffer, NULL, &hints, &info);
	if (error != 0) {
		strcpy(buffer, "getaddrinfo failed\n");
		return error;
	}
	strcpy(buffer, info->ai_canonname);
	freeaddrinfo(info);
	return error;
}