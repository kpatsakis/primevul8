static int jpc_sot_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_sot_t *sot = &ms->parms.sot;
	fprintf(out,
	  "tileno = %"PRIuFAST16"; len = %"PRIuFAST32"; partno = %d; numparts = %d\n",
	  sot->tileno, sot->len, sot->partno, sot->numparts);
	return 0;
}