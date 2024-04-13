pullf_create_mbuf_reader(PullFilter **mp_p, MBuf *src)
{
	return pullf_create(mp_p, &mbuf_reader, src, NULL);
}