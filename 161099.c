write_path_table(struct archive_write *a, int type_m, struct vdd *vdd)
{
	int depth, r;
	size_t path_table_size;

	r = ARCHIVE_OK;
	path_table_size = 0;
	for (depth = 0; depth < vdd->max_depth; depth++) {
		r = _write_path_table(a, type_m, depth, vdd);
		if (r < 0)
			return (r);
		path_table_size += r;
	}

	/* Write padding data. */
	path_table_size = path_table_size % PATH_TABLE_BLOCK_SIZE;
	if (path_table_size > 0)
		r = write_null(a, PATH_TABLE_BLOCK_SIZE - path_table_size);
	return (r);
}