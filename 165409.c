static int jpc_qcx_putcompparms(jpc_qcxcp_t *compparms, jpc_cstate_t *cstate,
  jas_stream_t *out)
{
	int i;

	/* Eliminate compiler warning about unused variables. */
	cstate = 0;

	jpc_putuint8(out, ((compparms->numguard & 7) << 5) | compparms->qntsty);
	for (i = 0; i < compparms->numstepsizes; ++i) {
		if (compparms->qntsty == JPC_QCX_NOQNT) {
			if (jpc_putuint8(out, JPC_QCX_GETEXPN(
			  compparms->stepsizes[i]) << 3)) {
				return -1;
			}
		} else {
			if (jpc_putuint16(out, compparms->stepsizes[i])) {
				return -1;
			}
		}
	}
	return 0;
}