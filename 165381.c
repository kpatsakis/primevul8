static int jpc_com_getparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *in)
{
	jpc_com_t *com = &ms->parms.com;

	/* Eliminate compiler warning about unused variables. */
	cstate = 0;

	if (jpc_getuint16(in, &com->regid)) {
		return -1;
	}
	com->len = ms->len - 2;
	if (com->len > 0) {
		if (!(com->data = jas_malloc(com->len))) {
			return -1;
		}
		if (jas_stream_read(in, com->data, com->len) != JAS_CAST(int, com->len)) {
			return -1;
		}
	} else {
		com->data = 0;
	}
	return 0;
}