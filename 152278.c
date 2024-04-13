cdf_dump_stream(const cdf_stream_t *sst)
{
	size_t ss = sst->sst_ss;
	cdf_dump(sst->sst_tab, ss * sst->sst_len);
}