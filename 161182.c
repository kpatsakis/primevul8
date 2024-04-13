isoent_alloc_path_table(struct archive_write *a, struct vdd *vdd,
    int max_depth)
{
	int i;

	vdd->max_depth = max_depth;
	vdd->pathtbl = malloc(sizeof(*vdd->pathtbl) * vdd->max_depth);
	if (vdd->pathtbl == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate memory");
		return (ARCHIVE_FATAL);
	}
	for (i = 0; i < vdd->max_depth; i++) {
		vdd->pathtbl[i].first = NULL;
		vdd->pathtbl[i].last = &(vdd->pathtbl[i].first);
		vdd->pathtbl[i].sorted = NULL;
		vdd->pathtbl[i].cnt = 0;
	}
	return (ARCHIVE_OK);
}