static void jpc_unk_destroyparms(jpc_ms_t *ms)
{
	jpc_unk_t *unk = &ms->parms.unk;
	if (unk->data) {
		jas_free(unk->data);
	}
}