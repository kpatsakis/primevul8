static int jpc_sop_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_sop_t *sop = &ms->parms.sop;
	fprintf(out, "seqno = %"PRIuFAST16";\n", sop->seqno);
	return 0;
}