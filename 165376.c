static void jpc_com_destroyparms(jpc_ms_t *ms)
{
	jpc_com_t *com = &ms->parms.com;
	if (com->data) {
		jas_free(com->data);
	}
}