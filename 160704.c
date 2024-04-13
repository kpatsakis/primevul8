GIT_INLINE(int) checkout_conflicts_cmp_entry(
	const char *path,
	const git_index_entry *entry)
{
	return strcmp((const char *)path, entry->path);
}