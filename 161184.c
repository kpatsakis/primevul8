wb_buffptr(struct archive_write *a)
{
	struct iso9660 *iso9660 = (struct iso9660 *)a->format_data;

	return (&(iso9660->wbuff[sizeof(iso9660->wbuff)
		- iso9660->wbuff_remaining]));
}