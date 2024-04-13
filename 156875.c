get_local_name(void)
{
	struct utsname name;
	struct addrinfo hints, *res = NULL;
	char *canonname = NULL;
	size_t len = 0;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;

	if (uname(&name) < 0)
		return NULL;

	if (getaddrinfo(name.nodename, NULL, &hints, &res) != 0)
		return NULL;

	if (res && res->ai_canonname) {
		len = strlen(res->ai_canonname);
		canonname = MALLOC(len + 1);
		if (canonname) {
			memcpy(canonname, res->ai_canonname, len);
		}
	}

	freeaddrinfo(res);

	return canonname;
}