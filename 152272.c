cdf_read_doc_summary_info(const cdf_info_t *info, const cdf_header_t *h,
    const cdf_sat_t *sat, const cdf_sat_t *ssat, const cdf_stream_t *sst,
    const cdf_dir_t *dir, cdf_stream_t *scn)
{
	return cdf_read_user_stream(info, h, sat, ssat, sst, dir,
	    "\05DocumentSummaryInformation", scn);
}