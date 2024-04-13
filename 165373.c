int jpc_putuint32(jas_stream_t *out, uint_fast32_t val)
{
	if (jas_stream_putc(out, (val >> 24) & 0xff) == EOF ||
	  jas_stream_putc(out, (val >> 16) & 0xff) == EOF ||
	  jas_stream_putc(out, (val >> 8) & 0xff) == EOF ||
	  jas_stream_putc(out, val & 0xff) == EOF) {
		return -1;
	}
	return 0;
}