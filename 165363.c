jpc_cstate_t *jpc_cstate_create()
{
	jpc_cstate_t *cstate;
	if (!(cstate = jas_malloc(sizeof(jpc_cstate_t)))) {
		return 0;
	}
	cstate->numcomps = 0;
	return cstate;
}