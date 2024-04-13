static int jpc_sot_getparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *in)
{
	jpc_sot_t *sot = &ms->parms.sot;

	/* Eliminate compiler warning about unused variables. */
	cstate = 0;

	if (jpc_getuint16(in, &sot->tileno) ||
	  jpc_getuint32(in, &sot->len) ||
	  jpc_getuint8(in, &sot->partno) ||
	  jpc_getuint8(in, &sot->numparts)) {
		return -1;
	}
	if (sot->tileno > 65534 || sot->len < 12 || sot->partno > 254 ||
	  sot->numparts > 255) {
		return -1;
	}
	if (jas_stream_eof(in)) {
		return -1;
	}
	return 0;
}