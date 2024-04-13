inet_sockaddrcmp(const struct sockaddr_storage *a, const struct sockaddr_storage *b)
{
	if (a->ss_family != b->ss_family)
		return -2;

	if (a->ss_family == AF_INET)
		return inet_inaddrcmp(a->ss_family,
				      &((struct sockaddr_in *) a)->sin_addr,
				      &((struct sockaddr_in *) b)->sin_addr);
	if (a->ss_family == AF_INET6)
		return inet_inaddrcmp(a->ss_family,
				      &((struct sockaddr_in6 *) a)->sin6_addr,
				      &((struct sockaddr_in6 *) b)->sin6_addr);
	return 0;
}