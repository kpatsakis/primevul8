static void jpc_ppm_destroyparms(jpc_ms_t *ms)
{
	jpc_ppm_t *ppm = &ms->parms.ppm;
	if (ppm->data) {
		jas_free(ppm->data);
	}
}