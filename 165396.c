static int jpc_ppt_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_ppt_t *ppt = &ms->parms.ppt;
	fprintf(out, "ind=%d; len = %d;\n", ppt->ind, ppt->len);
	if (ppt->len > 0) {
		fprintf(out, "data =\n");
		jas_memdump(out, ppt->data, ppt->len);
	}
	return 0;
}