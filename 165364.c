static int jpc_siz_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_siz_t *siz = &ms->parms.siz;
	unsigned int i;
	fprintf(out, "caps = 0x%02x;\n", siz->caps);
	fprintf(out, "width = %d; height = %d; xoff = %d; yoff = %d;\n",
	  siz->width, siz->height, siz->xoff, siz->yoff);
	fprintf(out, "tilewidth = %d; tileheight = %d; tilexoff = %d; "
	  "tileyoff = %d;\n", siz->tilewidth, siz->tileheight, siz->tilexoff,
	  siz->tileyoff);
	for (i = 0; i < siz->numcomps; ++i) {
		fprintf(out, "prec[%d] = %d; sgnd[%d] = %d; hsamp[%d] = %d; "
		  "vsamp[%d] = %d\n", i, siz->comps[i].prec, i,
		  siz->comps[i].sgnd, i, siz->comps[i].hsamp, i,
		  siz->comps[i].vsamp);
	}
	return 0;
}