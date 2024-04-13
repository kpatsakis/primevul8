static int jpc_poc_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_poc_t *poc = &ms->parms.poc;
	jpc_pocpchg_t *pchg;
	int pchgno;
	for (pchgno = 0, pchg = poc->pchgs; pchgno < poc->numpchgs; ++pchgno,
	  ++pchg) {
		if (jpc_putuint8(out, pchg->rlvlnostart) ||
		  ((cstate->numcomps > 256) ?
		  jpc_putuint16(out, pchg->compnostart) :
		  jpc_putuint8(out, pchg->compnostart)) ||
		  jpc_putuint16(out, pchg->lyrnoend) ||
		  jpc_putuint8(out, pchg->rlvlnoend) ||
		  ((cstate->numcomps > 256) ?
		  jpc_putuint16(out, pchg->compnoend) :
		  jpc_putuint8(out, pchg->compnoend)) ||
		  jpc_putuint8(out, pchg->prgord)) {
			return -1;
		}
	}
	return 0;
}