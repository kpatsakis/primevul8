static void jpc_qcd_destroyparms(jpc_ms_t *ms)
{
	jpc_qcd_t *qcd = &ms->parms.qcd;
	jpc_qcx_destroycompparms(&qcd->compparms);
}