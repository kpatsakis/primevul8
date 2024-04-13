void jpc_ms_destroy(jpc_ms_t *ms)
{
	if (ms->ops && ms->ops->destroyparms) {
		(*ms->ops->destroyparms)(ms);
	}
	jas_free(ms);
}