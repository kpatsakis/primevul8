static void jpc_qcx_destroycompparms(jpc_qcxcp_t *compparms)
{
	if (compparms->stepsizes) {
		jas_free(compparms->stepsizes);
	}
}