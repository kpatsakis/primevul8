static int jpc_siz_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_siz_t *siz = &ms->parms.siz;
	unsigned int i;
	fprintf(out, "caps = 0x%02"PRIxFAST16";\n", siz->caps);
	fprintf(out, "width = %"PRIuFAST32"; height = %"PRIuFAST32"; xoff = %"PRIuFAST32"; yoff = %" PRIuFAST32 ";\n",
	  siz->width, siz->height, siz->xoff, siz->yoff);
	fprintf(out, "tilewidth = %"PRIuFAST32"; tileheight = %"PRIuFAST32"; "
	  "tilexoff = %"PRIuFAST32"; tileyoff = %" PRIuFAST32 ";\n",
	  siz->tilewidth, siz->tileheight, siz->tilexoff, siz->tileyoff);
	fprintf(out, "numcomps = %"PRIuFAST16";\n", siz->numcomps);
	for (i = 0; i < siz->numcomps; ++i) {
		fprintf(out, "prec[%d] = %d; sgnd[%d] = %d; hsamp[%d] = %d; "
		  "vsamp[%d] = %d\n", i, siz->comps[i].prec, i,
		  siz->comps[i].sgnd, i, siz->comps[i].hsamp, i,
		  siz->comps[i].vsamp);
	}
	return 0;
}