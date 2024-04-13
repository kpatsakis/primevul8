static int jpc_cod_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_cod_t *cod = &ms->parms.cod;
	assert(cod->numlyrs > 0 && cod->compparms.numdlvls <= 32);
	assert(cod->compparms.numdlvls == cod->compparms.numrlvls - 1);
	if (jpc_putuint8(out, cod->compparms.csty) ||
	  jpc_putuint8(out, cod->prg) ||
	  jpc_putuint16(out, cod->numlyrs) ||
	  jpc_putuint8(out, cod->mctrans)) {
		return -1;
	}
	if (jpc_cox_putcompparms(ms, cstate, out,
	  (cod->csty & JPC_COX_PRT) != 0, &cod->compparms)) {
		return -1;
	}
	return 0;
}