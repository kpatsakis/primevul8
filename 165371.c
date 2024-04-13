static void jpc_poc_destroyparms(jpc_ms_t *ms)
{
	jpc_poc_t *poc = &ms->parms.poc;
	if (poc->pchgs) {
		jas_free(poc->pchgs);
	}
}