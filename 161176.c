isofile_hd_cmp_key(const struct archive_rb_node *n, const void *key)
{
	const struct hardlink *h = (const struct hardlink *)n;

	return (strcmp(archive_entry_pathname(h->file_list.first->entry),
		       (const char *)key));
}