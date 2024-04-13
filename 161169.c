_isoent_file_location(struct iso9660 *iso9660, struct isoent *isoent,
    int *symlocation)
{
	struct isoent **children;
	int n;

	if (isoent->children.cnt == 0)
		return;

	children = isoent->children_sorted;
	for (n = 0; n < isoent->children.cnt; n++) {
		struct isoent *np;
		struct isofile *file;

		np = children[n];
		if (np->dir)
			continue;
		if (np == iso9660->el_torito.boot)
			continue;
		file = np->file;
		if (file->boot || file->hardlink_target != NULL)
			continue;
		if (archive_entry_filetype(file->entry) == AE_IFLNK ||
		    file->content.size == 0) {
			/*
			 * Do not point a valid location.
			 * Make sure entry is not hardlink file.
			 */
			file->content.location = (*symlocation)--;
			continue;
		}

		file->write_content = 1;
	}
}