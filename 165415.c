static int jpc_unk_dumpparms(jpc_ms_t *ms, FILE *out)
{
	unsigned int i;
	jpc_unk_t *unk = &ms->parms.unk;
	for (i = 0; i < unk->len; ++i) {
		fprintf(out, "%02x ", unk->data[i]);
	}
	return 0;
}