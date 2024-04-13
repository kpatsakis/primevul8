static void jpc_cod_destroyparms(jpc_ms_t *ms)
{
	jpc_cod_t *cod = &ms->parms.cod;
	jpc_cox_destroycompparms(&cod->compparms);
}