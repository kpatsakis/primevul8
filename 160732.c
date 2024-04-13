GIT_INLINE(int) checkout_idxentry_cmp(
	const git_index_entry *a,
	const git_index_entry *b)
{
	if (!a && !b)
		return 0;
	else if (!a && b)
		return -1;
	else if(a && !b)
		return 1;
	else
		return strcmp(a->path, b->path);
}