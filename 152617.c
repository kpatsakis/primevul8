pull_from_mbuf(void *arg, PullFilter *src, int len,
			   uint8 **data_p, uint8 *buf, int buflen)
{
	MBuf	   *mbuf = arg;

	return mbuf_grab(mbuf, len, data_p);
}