static int jpc_sot_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_sot_t *sot = &ms->parms.sot;

	/* Eliminate compiler warning about unused variables. */
	cstate = 0;

	if (jpc_putuint16(out, sot->tileno) ||
	  jpc_putuint32(out, sot->len) ||
	  jpc_putuint8(out, sot->partno) ||
	  jpc_putuint8(out, sot->numparts)) {
		return -1;
	}
	return 0;
}