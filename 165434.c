static int jpc_cod_getparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *in)
{
	jpc_cod_t *cod = &ms->parms.cod;
	if (jpc_getuint8(in, &cod->csty)) {
		return -1;
	}
	if (jpc_getuint8(in, &cod->prg) ||
	  jpc_getuint16(in, &cod->numlyrs) ||
	  jpc_getuint8(in, &cod->mctrans)) {
		return -1;
	}
	if (jpc_cox_getcompparms(ms, cstate, in,
	  (cod->csty & JPC_COX_PRT) != 0, &cod->compparms)) {
		return -1;
	}
	if (jas_stream_eof(in)) {
		jpc_cod_destroyparms(ms);
		return -1;
	}
	return 0;
}