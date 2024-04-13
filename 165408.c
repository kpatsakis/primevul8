static int jpc_coc_getparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *in)
{
	jpc_coc_t *coc = &ms->parms.coc;
	uint_fast8_t tmp;
	if (cstate->numcomps <= 256) {
		if (jpc_getuint8(in, &tmp)) {
			return -1;
		}
		coc->compno = tmp;
	} else {
		if (jpc_getuint16(in, &coc->compno)) {
			return -1;
		}
	}
	if (jpc_getuint8(in, &coc->compparms.csty)) {
		return -1;
	}
	if (jpc_cox_getcompparms(ms, cstate, in,
	  (coc->compparms.csty & JPC_COX_PRT) != 0, &coc->compparms)) {
		return -1;
	}
	if (jas_stream_eof(in)) {
		return -1;
	}
	return 0;
}