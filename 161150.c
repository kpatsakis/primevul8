isoent_rr_move_dir(struct archive_write *a, struct isoent **rr_moved,
    struct isoent *curent, struct isoent **newent)
{
	struct iso9660 *iso9660 = a->format_data;
	struct isoent *rrmoved, *mvent, *np;

	if ((rrmoved = *rr_moved) == NULL) {
		struct isoent *rootent = iso9660->primary.rootent;
		/* There isn't rr_move entry.
		 * Create rr_move entry and insert it into the root entry.
		 */
		rrmoved = isoent_create_virtual_dir(a, iso9660, "rr_moved");
		if (rrmoved == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate memory");
			return (ARCHIVE_FATAL);
		}
		/* Add "rr_moved" entry to the root entry. */
		isoent_add_child_head(rootent, rrmoved);
		archive_entry_set_nlink(rootent->file->entry,
		    archive_entry_nlink(rootent->file->entry) + 1);
		/* Register "rr_moved" entry to second level pathtable. */
		path_table_add_entry(&(iso9660->primary.pathtbl[1]), rrmoved);
		/* Save rr_moved. */
		*rr_moved = rrmoved;
	}
	/*
	 * Make a clone of curent which is going to be relocated
	 * to rr_moved.
	 */
	mvent = isoent_clone(curent);
	if (mvent == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate memory");
		return (ARCHIVE_FATAL);
	}
	/* linking..  and use for creating "CL", "PL" and "RE" */
	mvent->rr_parent = curent->parent;
	curent->rr_child = mvent;
	/*
	 * Move subdirectories from the curent to mvent
	 */
	if (curent->children.first != NULL) {
		*mvent->children.last = curent->children.first;
		mvent->children.last = curent->children.last;
	}
	for (np = mvent->children.first; np != NULL; np = np->chnext)
		np->parent = mvent;
	mvent->children.cnt = curent->children.cnt;
	curent->children.cnt = 0;
	curent->children.first = NULL;
	curent->children.last = &curent->children.first;

	if (curent->subdirs.first != NULL) {
		*mvent->subdirs.last = curent->subdirs.first;
		mvent->subdirs.last = curent->subdirs.last;
	}
	mvent->subdirs.cnt = curent->subdirs.cnt;
	curent->subdirs.cnt = 0;
	curent->subdirs.first = NULL;
	curent->subdirs.last = &curent->subdirs.first;

	/*
	 * The mvent becomes a child of the rr_moved entry.
	 */
	isoent_add_child_tail(rrmoved, mvent);
	archive_entry_set_nlink(rrmoved->file->entry,
	    archive_entry_nlink(rrmoved->file->entry) + 1);
	/*
	 * This entry which relocated to the rr_moved directory
	 * has to set the flag as a file.
	 * See also RRIP 4.1.5.1 Description of the "CL" System Use Entry.
	 */
	curent->dir = 0;

	*newent = mvent;

	return (ARCHIVE_OK);
}