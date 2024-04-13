static int jpc_ppm_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_ppm_t *ppm = &ms->parms.ppm;
	fprintf(out, "ind=%d; len = %"PRIuFAST16";\n", ppm->ind, ppm->len);
	if (ppm->len > 0) {
		fprintf(out, "data =\n");
		jas_memdump(out, ppm->data, ppm->len);
	}
	return 0;
}