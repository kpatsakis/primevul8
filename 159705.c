static bool is_io_l2cap_based(int fd)
{
	int domain;
	int proto;
	int err;
	socklen_t len;

	domain = 0;
	len = sizeof(domain);
	err = getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &domain, &len);
	if (err < 0)
		return false;

	if (domain != AF_BLUETOOTH)
		return false;

	proto = 0;
	len = sizeof(proto);
	err = getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &proto, &len);
	if (err < 0)
		return false;

	return proto == BTPROTO_L2CAP;
}