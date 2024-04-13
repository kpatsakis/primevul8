write_null(struct archive_write *a, size_t size)
{
	size_t remaining;
	unsigned char *p, *old;
	int r;

	remaining = wb_remaining(a);
	p = wb_buffptr(a);
	if (size <= remaining) {
		memset(p, 0, size);
		return (wb_consume(a, size));
	}
	memset(p, 0, remaining);
	r = wb_consume(a, remaining);
	if (r != ARCHIVE_OK)
		return (r);
	size -= remaining;
	old = p;
	p = wb_buffptr(a);
	memset(p, 0, old - p);
	remaining = wb_remaining(a);
	while (size) {
		size_t wsize = size;

		if (wsize > remaining)
			wsize = remaining;
		r = wb_consume(a, wsize);
		if (r != ARCHIVE_OK)
			return (r);
		size -= wsize;
	}
	return (ARCHIVE_OK);
}