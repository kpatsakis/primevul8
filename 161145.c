calculate_directory_descriptors(struct iso9660 *iso9660, struct vdd *vdd,
    struct isoent *isoent, int depth)
{
	struct isoent **enttbl;
	int bs, block, i;

	block = 1;
	bs = get_dir_rec_size(iso9660, isoent, DIR_REC_SELF, vdd->vdd_type);
	bs += get_dir_rec_size(iso9660, isoent, DIR_REC_PARENT, vdd->vdd_type);

	if (isoent->children.cnt <= 0 || (vdd->vdd_type != VDD_JOLIET &&
	    !iso9660->opt.rr && depth + 1 >= vdd->max_depth))
		return (block);

	enttbl = isoent->children_sorted;
	for (i = 0; i < isoent->children.cnt; i++) {
		struct isoent *np = enttbl[i];
		struct isofile *file;

		file = np->file;
		if (file->hardlink_target != NULL)
			file = file->hardlink_target;
		file->cur_content = &(file->content);
		do {
			int dr_l;

			dr_l = get_dir_rec_size(iso9660, np, DIR_REC_NORMAL,
			    vdd->vdd_type);
			if ((bs + dr_l) > LOGICAL_BLOCK_SIZE) {
				block ++;
				bs = dr_l;
			} else
				bs += dr_l;
			file->cur_content = file->cur_content->next;
		} while (file->cur_content != NULL);
	}
	return (block);
}