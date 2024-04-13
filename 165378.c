static void jpc_crg_destroyparms(jpc_ms_t *ms)
{
	jpc_crg_t *crg = &ms->parms.crg;
	if (crg->comps) {
		jas_free(crg->comps);
	}
}