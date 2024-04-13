cdf_zero_stream(cdf_stream_t *scn)
{
	scn->sst_len = 0;
	scn->sst_dirlen = 0;
	scn->sst_ss = 0;
	free(scn->sst_tab);
	scn->sst_tab = NULL;
	return -1;
}