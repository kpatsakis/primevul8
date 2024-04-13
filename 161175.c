isoent_setup_directory_location(struct iso9660 *iso9660, int location,
    struct vdd *vdd)
{
	struct isoent *np;
	int depth;

	vdd->total_dir_block = 0;
	depth = 0;
	np = vdd->rootent;
	do {
		int block;

		np->dir_block = calculate_directory_descriptors(
		    iso9660, vdd, np, depth);
		vdd->total_dir_block += np->dir_block;
		np->dir_location = location;
		location += np->dir_block;
		block = extra_setup_location(np, location);
		vdd->total_dir_block += block;
		location += block;

		if (np->subdirs.first != NULL && depth + 1 < vdd->max_depth) {
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
}