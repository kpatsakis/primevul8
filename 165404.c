static int jpc_qcd_getparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *in)
{
	jpc_qcxcp_t *compparms = &ms->parms.qcd.compparms;
	return jpc_qcx_getcompparms(compparms, cstate, in, ms->len);
}