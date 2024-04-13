static int jpc_rgn_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_rgn_t *rgn = &ms->parms.rgn;
	if (cstate->numcomps <= 256) {
		if (jpc_putuint8(out, rgn->compno)) {
			return -1;
		}
	} else {
		if (jpc_putuint16(out, rgn->compno)) {
			return -1;
		}
	}
	if (jpc_putuint8(out, rgn->roisty) ||
	  jpc_putuint8(out, rgn->roishift)) {
		return -1;
	}
	return 0;
}