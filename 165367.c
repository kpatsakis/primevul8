static int jpc_crg_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_crg_t *crg = &ms->parms.crg;
	int compno;
	jpc_crgcomp_t *comp;
	for (compno = 0, comp = crg->comps; compno < crg->numcomps; ++compno,
	  ++comp) {
		fprintf(out, "hoff[%d] = %d; voff[%d] = %d\n", compno,
		  comp->hoff, compno, comp->voff);
	}
	return 0;
}