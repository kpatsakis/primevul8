static int jpc_qcd_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_qcxcp_t *compparms = &ms->parms.qcd.compparms;
	return jpc_qcx_putcompparms(compparms, cstate, out);
}