int jpc_getdata(jas_stream_t *in, jas_stream_t *out, long len)
{
	return jas_stream_copy(out, in, len);
}