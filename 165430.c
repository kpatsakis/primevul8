static void jpc_qcc_destroyparms(jpc_ms_t *ms)
{
	jpc_qcc_t *qcc = &ms->parms.qcc;
	jpc_qcx_destroycompparms(&qcc->compparms);
}