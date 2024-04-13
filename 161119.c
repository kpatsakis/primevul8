isofile_hd_cmp_node(const struct archive_rb_node *n1,
    const struct archive_rb_node *n2)
{
	const struct hardlink *h1 = (const struct hardlink *)n1;
	const struct hardlink *h2 = (const struct hardlink *)n2;

	return (strcmp(archive_entry_pathname(h1->file_list.first->entry),
		       archive_entry_pathname(h2->file_list.first->entry)));
}