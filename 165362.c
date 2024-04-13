int jpc_getuint8(jas_stream_t *in, uint_fast8_t *val)
{
	int c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	if (val) {
		*val = c;
	}
	return 0;
}