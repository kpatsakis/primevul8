static void jpc_coc_destroyparms(jpc_ms_t *ms)
{
	jpc_coc_t *coc = &ms->parms.coc;
	jpc_cox_destroycompparms(&coc->compparms);
}