static int jpc_coc_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_coc_t *coc = &ms->parms.coc;
	fprintf(out, "compno = %d; csty = 0x%02x; numdlvls = %d;\n",
	  coc->compno, coc->compparms.csty, coc->compparms.numdlvls);
	fprintf(out, "cblkwidthval = %d; cblkheightval = %d; "
	  "cblksty = 0x%02x; qmfbid = %d;\n", coc->compparms.cblkwidthval,
	  coc->compparms.cblkheightval, coc->compparms.cblksty, coc->compparms.qmfbid);
	return 0;
}