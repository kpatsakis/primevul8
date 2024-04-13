static int jpc_qcc_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_qcc_t *qcc = &ms->parms.qcc;
	int i;
	fprintf(out, "compno = %"PRIuFAST16"; qntsty = %d; numguard = %d; "
	  "numstepsizes = %d\n", qcc->compno, qcc->compparms.qntsty, qcc->compparms.numguard,
	  qcc->compparms.numstepsizes);
	for (i = 0; i < qcc->compparms.numstepsizes; ++i) {
		fprintf(out, "expn[%d] = 0x%04x; mant[%d] = 0x%04x;\n",
		  i, (unsigned) JPC_QCX_GETEXPN(qcc->compparms.stepsizes[i]),
		  i, (unsigned) JPC_QCX_GETMANT(qcc->compparms.stepsizes[i]));
	}
	return 0;
}