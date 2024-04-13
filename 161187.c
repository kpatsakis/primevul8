calculate_path_table_size(struct vdd *vdd)
{
	int depth, size;
	struct path_table *pt;

	pt = vdd->pathtbl;
	size = 0;
	for (depth = 0; depth < vdd->max_depth; depth++) {
		struct isoent **ptbl;
		int i, cnt;

		if ((cnt = pt[depth].cnt) == 0)
			break;

		ptbl = pt[depth].sorted;
		for (i = 0; i < cnt; i++) {
			int len;

			if (ptbl[i]->identifier == NULL)
				len = 1; /* root directory */
			else
				len = ptbl[i]->id_len;
			if (len & 0x01)
				len++; /* Padding Field */
			size += 8 + len;
		}
	}
	vdd->path_table_size = size;
	vdd->path_table_block =
	    ((size + PATH_TABLE_BLOCK_SIZE -1) /
	    PATH_TABLE_BLOCK_SIZE) *
	    (PATH_TABLE_BLOCK_SIZE / LOGICAL_BLOCK_SIZE);
}