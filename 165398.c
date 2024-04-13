static int jpc_qcc_getparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *in)
{
	jpc_qcc_t *qcc = &ms->parms.qcc;
	uint_fast8_t tmp;
	int len;
	len = ms->len;
	if (cstate->numcomps <= 256) {
		if (jpc_getuint8(in, &tmp)) {
			return -1;
		}
		qcc->compno = tmp;
		--len;
	} else {
		if (jpc_getuint16(in, &qcc->compno)) {
			return -1;
		}
		len -= 2;
	}
	if (jpc_qcx_getcompparms(&qcc->compparms, cstate, in, len)) {
		return -1;
	}
	if (jas_stream_eof(in)) {
		jpc_qcc_destroyparms(ms);
		return -1;
	}
	return 0;
}