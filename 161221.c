isoent_rr_move(struct archive_write *a)
{
	struct iso9660 *iso9660 = a->format_data;
	struct path_table *pt;
	struct isoent *rootent, *rr_moved;
	struct isoent *np, *last;
	int r;

	pt = &(iso9660->primary.pathtbl[MAX_DEPTH-1]);
	/* Theare aren't level 8 directories reaching a deepr level. */
	if (pt->cnt == 0)
		return (ARCHIVE_OK);

	rootent = iso9660->primary.rootent;
	/* If "rr_moved" directory is already existing,
	 * we have to use it. */
	rr_moved = isoent_find_child(rootent, "rr_moved");
	if (rr_moved != NULL &&
	    rr_moved != rootent->children.first) {
		/*
		 * It's necessary that rr_move is the first entry
		 * of the root.
		 */
		/* Remove "rr_moved" entry from children chain. */
		isoent_remove_child(rootent, rr_moved);

		/* Add "rr_moved" entry into the head of children chain. */
		isoent_add_child_head(rootent, rr_moved);
	}

	/*
	 * Check level 8 path_table.
	 * If find out sub directory entries, that entries move to rr_move.
	 */
	np = pt->first;
	while (np != NULL) {
		last = path_table_last_entry(pt);
		for (; np != NULL; np = np->ptnext) {
			struct isoent *mvent;
			struct isoent *newent;

			if (!np->dir)
				continue;
			for (mvent = np->subdirs.first;
			    mvent != NULL; mvent = mvent->drnext) {
				r = isoent_rr_move_dir(a, &rr_moved,
				    mvent, &newent);
				if (r < 0)
					return (r);
				isoent_collect_dirs(&(iso9660->primary),
				    newent, 2);
			}
		}
		/* If new entries are added to level 8 path_talbe,
		 * its sub directory entries move to rr_move too.
		 */
		np = last->ptnext;
	}

	return (ARCHIVE_OK);
}