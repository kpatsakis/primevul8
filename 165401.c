static int jpc_qcc_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_qcc_t *qcc = &ms->parms.qcc;
	if (cstate->numcomps <= 256) {
		if (jpc_putuint8(out, qcc->compno)) {
			return -1;
		}
	} else {
		if (jpc_putuint16(out, qcc->compno)) {
			return -1;
		}
	}
	if (jpc_qcx_putcompparms(&qcc->compparms, cstate, out)) {
		return -1;
	}
	return 0;
}