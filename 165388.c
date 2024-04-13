static int jpc_unk_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	/* Eliminate compiler warning about unused variables. */
	cstate = 0;
	ms = 0;
	out = 0;

	/* If this function is called, we are trying to write an unsupported
	  type of marker segment.  Return with an error indication.  */
	return -1;
}