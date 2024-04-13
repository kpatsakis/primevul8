static int jpc_com_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_com_t *com = &ms->parms.com;

	/* Eliminate compiler warning about unused variables. */
	cstate = 0;

	if (jpc_putuint16(out, com->regid)) {
		return -1;
	}
	if (jas_stream_write(out, com->data, com->len) != JAS_CAST(int, com->len)) {
		return -1;
	}
	return 0;
}