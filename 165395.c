static int jpc_sop_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_sop_t *sop = &ms->parms.sop;

	/* Eliminate compiler warning about unused variable. */
	cstate = 0;

	if (jpc_putuint16(out, sop->seqno)) {
		return -1;
	}
	return 0;
}