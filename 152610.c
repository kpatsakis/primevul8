push_into_mbuf(PushFilter *next, void *arg, const uint8 *data, int len)
{
	int			res = 0;
	MBuf	   *mbuf = arg;

	if (len > 0)
		res = mbuf_append(mbuf, data, len);
	return res < 0 ? res : 0;
}