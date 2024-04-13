cdf_check_stream(const cdf_stream_t *sst, const cdf_header_t *h)
{
	size_t ss = sst->sst_dirlen < h->h_min_size_standard_stream ?
	    CDF_SHORT_SEC_SIZE(h) : CDF_SEC_SIZE(h);
	assert(ss == sst->sst_ss);
	return sst->sst_ss;
}