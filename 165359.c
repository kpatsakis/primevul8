static int jpc_rgn_getparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *in)
{
	jpc_rgn_t *rgn = &ms->parms.rgn;
	uint_fast8_t tmp;
	if (cstate->numcomps <= 256) {
		if (jpc_getuint8(in, &tmp)) {
			return -1;
		}
		rgn->compno = tmp;
	} else {
		if (jpc_getuint16(in, &rgn->compno)) {
			return -1;
		}
	}
	if (jpc_getuint8(in, &rgn->roisty) ||
	  jpc_getuint8(in, &rgn->roishift)) {
		return -1;
	}
	return 0;
}