isofile_connect_hardlink_files(struct iso9660 *iso9660)
{
	struct archive_rb_node *n;
	struct hardlink *hl;
	struct isofile *target, *nf;

	ARCHIVE_RB_TREE_FOREACH(n, &(iso9660->hardlink_rbtree)) {
		hl = (struct hardlink *)n;

		/* The first entry must be a hardlink target. */
		target = hl->file_list.first;
		archive_entry_set_nlink(target->entry, hl->nlink);
		/* Set a hardlink target to reference entries. */
		for (nf = target->hlnext;
		    nf != NULL; nf = nf->hlnext) {
			nf->hardlink_target = target;
			archive_entry_set_nlink(nf->entry, hl->nlink);
		}
	}
}