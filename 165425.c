static void jpc_ppt_destroyparms(jpc_ms_t *ms)
{
	jpc_ppt_t *ppt = &ms->parms.ppt;
	if (ppt->data) {
		jas_free(ppt->data);
	}
}