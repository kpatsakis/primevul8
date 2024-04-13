static int jpc_cod_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_cod_t *cod = &ms->parms.cod;
	int i;
	fprintf(out, "csty = 0x%02x;\n", cod->compparms.csty);
	fprintf(out, "numdlvls = %d; qmfbid = %d; mctrans = %d\n",
	  cod->compparms.numdlvls, cod->compparms.qmfbid, cod->mctrans);
	fprintf(out, "prg = %d; numlyrs = %"PRIuFAST16";\n",
	  cod->prg, cod->numlyrs);
	fprintf(out, "cblkwidthval = %d; cblkheightval = %d; "
	  "cblksty = 0x%02x;\n", cod->compparms.cblkwidthval, cod->compparms.cblkheightval,
	  cod->compparms.cblksty);
	if (cod->csty & JPC_COX_PRT) {
		for (i = 0; i < cod->compparms.numrlvls; ++i) {
			jas_eprintf("prcwidth[%d] = %d, prcheight[%d] = %d\n",
			  i, cod->compparms.rlvls[i].parwidthval,
			  i, cod->compparms.rlvls[i].parheightval);
		}
	}
	return 0;
}