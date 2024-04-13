static uint8_t io_get_type(int fd)
{
	struct sockaddr_l2 src;
	socklen_t len;

	if (!is_io_l2cap_based(fd))
		return BT_ATT_LOCAL;

	len = sizeof(src);
	memset(&src, 0, len);
	if (getsockname(fd, (void *)&src, &len) < 0)
		return -errno;

	if (src.l2_bdaddr_type == BDADDR_BREDR)
		return BT_ATT_BREDR;

	return BT_ATT_LE;
}