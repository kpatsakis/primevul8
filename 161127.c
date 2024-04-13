idr_ensure_poolsize(struct archive_write *a, struct idr *idr,
    int cnt)
{

	if (idr->pool_size < cnt) {
		void *p;
		const int bk = (1 << 7) - 1;
		int psize;

		psize = (cnt + bk) & ~bk;
		p = realloc(idr->idrent_pool, sizeof(struct idrent) * psize);
		if (p == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate memory");
			return (ARCHIVE_FATAL);
		}
		idr->idrent_pool = (struct idrent *)p;
		idr->pool_size = psize;
	}
	return (ARCHIVE_OK);
}