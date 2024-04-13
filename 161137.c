_write_path_table(struct archive_write *a, int type_m, int depth,
    struct vdd *vdd)
{
	unsigned char *bp, *wb;
	struct isoent **ptbl;
	size_t wbremaining;
	int i, r, wsize;

	if (vdd->pathtbl[depth].cnt == 0)
		return (0);

	wsize = 0;
	wb = wb_buffptr(a);
	wbremaining = wb_remaining(a);
	bp = wb - 1;
	ptbl = vdd->pathtbl[depth].sorted;
	for (i = 0; i < vdd->pathtbl[depth].cnt; i++) {
		struct isoent *np;
		size_t len;

		np = ptbl[i];
		if (np->identifier == NULL)
			len = 1; /* root directory */
		else
			len = np->id_len;
		if (wbremaining - ((bp+1) - wb) < (len + 1 + 8)) {
			r = wb_consume(a, (bp+1) - wb);
			if (r < 0)
				return (r);
			wb = wb_buffptr(a);
			wbremaining = wb_remaining(a);
			bp = wb -1;
		}
		/* Length of Directory Identifier */
		set_num_711(bp+1, (unsigned char)len);
		/* Extended Attribute Record Length */
		set_num_711(bp+2, 0);
		/* Location of Extent */
		if (type_m)
			set_num_732(bp+3, np->dir_location);
		else
			set_num_731(bp+3, np->dir_location);
		/* Parent Directory Number */
		if (type_m)
			set_num_722(bp+7, np->parent->dir_number);
		else
			set_num_721(bp+7, np->parent->dir_number);
		/* Directory Identifier */
		if (np->identifier == NULL)
			bp[9] = 0;
		else
			memcpy(&bp[9], np->identifier, len);
		if (len & 0x01) {
			/* Padding Field */
			bp[9+len] = 0;
			len++;
		}
		wsize += 8 + (int)len;
		bp += 8 + len;
	}
	if ((bp + 1) > wb) {
		r = wb_consume(a, (bp+1)-wb);
		if (r < 0)
			return (r);
	}
	return (wsize);
}