wb_write_out(struct archive_write *a)
{
	struct iso9660 *iso9660 = (struct iso9660 *)a->format_data;
	size_t wsize, nw;
	int r;

	wsize = sizeof(iso9660->wbuff) - iso9660->wbuff_remaining;
	nw = wsize % LOGICAL_BLOCK_SIZE;
	if (iso9660->wbuff_type == WB_TO_STREAM)
		r = __archive_write_output(a, iso9660->wbuff, wsize - nw);
	else
		r = write_to_temp(a, iso9660->wbuff, wsize - nw);
	/* Increase the offset. */
	iso9660->wbuff_offset += wsize - nw;
	if (iso9660->wbuff_offset > iso9660->wbuff_written)
		iso9660->wbuff_written = iso9660->wbuff_offset;
	iso9660->wbuff_remaining = sizeof(iso9660->wbuff);
	if (nw) {
		iso9660->wbuff_remaining -= nw;
		memmove(iso9660->wbuff, iso9660->wbuff + wsize - nw, nw);
	}
	return (r);
}