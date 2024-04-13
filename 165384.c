static int jpc_coc_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_coc_t *coc = &ms->parms.coc;
	assert(coc->compparms.numdlvls <= 32);
	if (cstate->numcomps <= 256) {
		if (jpc_putuint8(out, coc->compno)) {
			return -1;
		}
	} else {
		if (jpc_putuint16(out, coc->compno)) {
			return -1;
		}
	}
	if (jpc_putuint8(out, coc->compparms.csty)) {
		return -1;
	}
	if (jpc_cox_putcompparms(ms, cstate, out,
	  (coc->compparms.csty & JPC_COX_PRT) != 0, &coc->compparms)) {
		return -1;
	}
	return 0;
}