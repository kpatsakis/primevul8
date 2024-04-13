_write_directory_descriptors(struct archive_write *a, struct vdd *vdd,
    struct isoent *isoent, int depth)
{
	struct iso9660 *iso9660 = a->format_data;
	struct isoent **enttbl;
	unsigned char *p, *wb;
	int i, r;
	int dr_l;

	p = wb = wb_buffptr(a);
#define WD_REMAINING	(LOGICAL_BLOCK_SIZE - (p - wb))
	p += set_directory_record(p, WD_REMAINING, isoent,
	    iso9660, DIR_REC_SELF, vdd->vdd_type);
	p += set_directory_record(p, WD_REMAINING, isoent,
	    iso9660, DIR_REC_PARENT, vdd->vdd_type);

	if (isoent->children.cnt <= 0 || (vdd->vdd_type != VDD_JOLIET &&
	    !iso9660->opt.rr && depth + 1 >= vdd->max_depth)) {
		memset(p, 0, WD_REMAINING);
		return (wb_consume(a, LOGICAL_BLOCK_SIZE));
	}

	enttbl = isoent->children_sorted;
	for (i = 0; i < isoent->children.cnt; i++) {
		struct isoent *np = enttbl[i];
		struct isofile *file = np->file;

		if (file->hardlink_target != NULL)
			file = file->hardlink_target;
		file->cur_content = &(file->content);
		do {
			dr_l = set_directory_record(p, WD_REMAINING,
			    np, iso9660, DIR_REC_NORMAL,
			    vdd->vdd_type);
			if (dr_l == 0) {
				memset(p, 0, WD_REMAINING);
				r = wb_consume(a, LOGICAL_BLOCK_SIZE);
				if (r < 0)
					return (r);
				p = wb = wb_buffptr(a);
				dr_l = set_directory_record(p,
				    WD_REMAINING, np, iso9660,
				    DIR_REC_NORMAL, vdd->vdd_type);
			}
			p += dr_l;
			file->cur_content = file->cur_content->next;
		} while (file->cur_content != NULL);
	}
	memset(p, 0, WD_REMAINING);
	return (wb_consume(a, LOGICAL_BLOCK_SIZE));
}