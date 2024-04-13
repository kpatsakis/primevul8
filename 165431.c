int jpc_putdata(jas_stream_t *out, jas_stream_t *in, long len)
{
	return jas_stream_copy(out, in, len);
}