isoent_setup_file_location(struct iso9660 *iso9660, int location)
{
	struct isoent *isoent;
	struct isoent *np;
	struct isofile *file;
	size_t size;
	int block;
	int depth;
	int joliet;
	int symlocation;
	int total_block;

	iso9660->total_file_block = 0;
	if ((isoent = iso9660->el_torito.catalog) != NULL) {
		isoent->file->content.location = location;
		block = (int)((archive_entry_size(isoent->file->entry) +
		    LOGICAL_BLOCK_SIZE -1) >> LOGICAL_BLOCK_BITS);
		location += block;
		iso9660->total_file_block += block;
	}
	if ((isoent = iso9660->el_torito.boot) != NULL) {
		isoent->file->content.location = location;
		size = fd_boot_image_size(iso9660->el_torito.media_type);
		if (size == 0)
			size = (size_t)archive_entry_size(isoent->file->entry);
		block = ((int)size + LOGICAL_BLOCK_SIZE -1)
		    >> LOGICAL_BLOCK_BITS;
		location += block;
		iso9660->total_file_block += block;
		isoent->file->content.blocks = block;
	}

	depth = 0;
	symlocation = -16;
	if (!iso9660->opt.rr && iso9660->opt.joliet) {
		joliet = 1;
		np = iso9660->joliet.rootent;
	} else {
		joliet = 0;
		np = iso9660->primary.rootent;
	}
	do {
		_isoent_file_location(iso9660, np, &symlocation);

		if (np->subdirs.first != NULL &&
		    (joliet ||
		    ((iso9660->opt.rr == OPT_RR_DISABLED &&
		      depth + 2 < iso9660->primary.max_depth) ||
		     (iso9660->opt.rr &&
		      depth + 1 < iso9660->primary.max_depth)))) {
			/* Enter to sub directories. */
			np = np->subdirs.first;
			depth++;
			continue;
		}
		while (np != np->parent) {
			if (np->drnext == NULL) {
				/* Return to the parent directory. */
				np = np->parent;
				depth--;
			} else {
				np = np->drnext;
				break;
			}
		}
	} while (np != np->parent);

	total_block = 0;
	for (file = iso9660->data_file_list.first;
	    file != NULL; file = file->datanext) {

		if (!file->write_content)
			continue;

		file->cur_content = &(file->content);
		do {
			file->cur_content->location = location;
			location += file->cur_content->blocks;
			total_block += file->cur_content->blocks;
			/* Next fragument */
			file->cur_content = file->cur_content->next;
		} while (file->cur_content != NULL);
	}
	iso9660->total_file_block += total_block;
}