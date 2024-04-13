static void jpc_siz_destroyparms(jpc_ms_t *ms)
{
	jpc_siz_t *siz = &ms->parms.siz;
	if (siz->comps) {
		jas_free(siz->comps);
	}
}