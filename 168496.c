static void *read_index_data(const char *path)
{
	int pos, len;
	unsigned long sz;
	enum object_type type;
	void *data;

	len = strlen(path);
	pos = cache_name_pos(path, len);
	if (pos < 0) {
		/*
		 * We might be in the middle of a merge, in which
		 * case we would read stage #2 (ours).
		 */
		int i;
		for (i = -pos - 1;
		     (pos < 0 && i < active_nr &&
		      !strcmp(active_cache[i]->name, path));
		     i++)
			if (ce_stage(active_cache[i]) == 2)
				pos = i;
	}
	if (pos < 0)
		return NULL;
	data = read_sha1_file(active_cache[pos]->sha1, &type, &sz);
	if (!data || type != OBJ_BLOB) {
		free(data);
		return NULL;
	}
	return data;
}