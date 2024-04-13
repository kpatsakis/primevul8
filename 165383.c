int jpc_getuint16(jas_stream_t *in, uint_fast16_t *val)
{
	uint_fast16_t v;
	int c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = (v << 8) | c;
	if (val) {
		*val = v;
	}
	return 0;
}