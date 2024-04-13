int jpc_getuint32(jas_stream_t *in, uint_fast32_t *val)
{
	uint_fast32_t v;
	int c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = (v << 8) | c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = (v << 8) | c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = (v << 8) | c;
	if (val) {
		*val = v;
	}
	return 0;
}