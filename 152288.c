cdf_read_user_stream(const cdf_info_t *info, const cdf_header_t *h,
    const cdf_sat_t *sat, const cdf_sat_t *ssat, const cdf_stream_t *sst,
    const cdf_dir_t *dir, const char *name, cdf_stream_t *scn)
{
	const cdf_directory_t *d;
	int i = cdf_find_stream(dir, name, CDF_DIR_TYPE_USER_STREAM);

	if (i <= 0) {
		memset(scn, 0, sizeof(*scn));
		return -1;
	}

	d = &dir->dir_tab[i - 1];
	return cdf_read_sector_chain(info, h, sat, ssat, sst,
	    d->d_stream_first_sector, d->d_size, scn);
}