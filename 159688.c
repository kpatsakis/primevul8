static uint16_t io_get_mtu(int fd)
{
	socklen_t len;
	struct l2cap_options l2o;

	len = sizeof(l2o);
	if (!getsockopt(fd, SOL_L2CAP, L2CAP_OPTIONS, &l2o, &len))
		return l2o.omtu;

	if (!getsockopt(fd, SOL_BLUETOOTH, BT_SNDMTU, &l2o.omtu, &len))
		return l2o.omtu;

	return 0;
}