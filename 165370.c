static int jpc_cox_putcompparms(jpc_ms_t *ms, jpc_cstate_t *cstate,
  jas_stream_t *out, int prtflag, jpc_coxcp_t *compparms)
{
	int i;
	assert(compparms->numdlvls <= 32);

	/* Eliminate compiler warning about unused variables. */
	ms = 0;
	cstate = 0;

	if (jpc_putuint8(out, compparms->numdlvls) ||
	  jpc_putuint8(out, compparms->cblkwidthval) ||
	  jpc_putuint8(out, compparms->cblkheightval) ||
	  jpc_putuint8(out, compparms->cblksty) ||
	  jpc_putuint8(out, compparms->qmfbid)) {
		return -1;
	}
	if (prtflag) {
		for (i = 0; i < compparms->numrlvls; ++i) {
			if (jpc_putuint8(out,
			  ((compparms->rlvls[i].parheightval & 0xf) << 4) |
			  (compparms->rlvls[i].parwidthval & 0xf))) {
				return -1;
			}
		}
	}
	return 0;
}