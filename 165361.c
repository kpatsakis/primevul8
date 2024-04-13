static int jpc_poc_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_poc_t *poc = &ms->parms.poc;
	jpc_pocpchg_t *pchg;
	int pchgno;
	for (pchgno = 0, pchg = poc->pchgs; pchgno < poc->numpchgs;
	  ++pchgno, ++pchg) {
		fprintf(out, "po[%d] = %d; ", pchgno, pchg->prgord);
		fprintf(out, "cs[%d] = %d; ce[%d] = %d; ",
		  pchgno, pchg->compnostart, pchgno, pchg->compnoend);
		fprintf(out, "rs[%d] = %d; re[%d] = %d; ",
		  pchgno, pchg->rlvlnostart, pchgno, pchg->rlvlnoend);
		fprintf(out, "le[%d] = %d\n", pchgno, pchg->lyrnoend);
	}
	return 0;
}