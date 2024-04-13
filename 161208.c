isofile_register_hardlink(struct archive_write *a, struct isofile *file)
{
	struct iso9660 *iso9660 = a->format_data;
	struct hardlink *hl;
	const char *pathname;

	archive_entry_set_nlink(file->entry, 1);
	pathname = archive_entry_hardlink(file->entry);
	if (pathname == NULL) {
		/* This `file` is a hardlink target. */
		hl = malloc(sizeof(*hl));
		if (hl == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate memory");
			return (ARCHIVE_FATAL);
		}
		hl->nlink = 1;
		/* A hardlink target must be the first position. */
		file->hlnext = NULL;
		hl->file_list.first = file;
		hl->file_list.last = &(file->hlnext);
		__archive_rb_tree_insert_node(&(iso9660->hardlink_rbtree),
		    (struct archive_rb_node *)hl);
	} else {
		hl = (struct hardlink *)__archive_rb_tree_find_node(
		    &(iso9660->hardlink_rbtree), pathname);
		if (hl != NULL) {
			/* Insert `file` entry into the tail. */
			file->hlnext = NULL;
			*hl->file_list.last = file;
			hl->file_list.last = &(file->hlnext);
			hl->nlink++;
		}
		archive_entry_unset_size(file->entry);
	}

	return (ARCHIVE_OK);
}