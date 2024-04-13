static int jpc_crg_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_crg_t *crg = &ms->parms.crg;
	int compno;
	jpc_crgcomp_t *comp;

	/* Eliminate compiler warning about unused variables. */
	cstate = 0;

	for (compno = 0, comp = crg->comps; compno < crg->numcomps; ++compno,
	  ++comp) {
		if (jpc_putuint16(out, comp->hoff) ||
		  jpc_putuint16(out, comp->voff)) {
			return -1;
		}
	}
	return 0;
}