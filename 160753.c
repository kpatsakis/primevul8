GIT_INLINE(bool) is_workdir_base_or_new(
	const git_oid *workdir_id,
	const git_diff_file *baseitem,
	const git_diff_file *newitem)
{
	return (git_oid__cmp(&baseitem->id, workdir_id) == 0 ||
		git_oid__cmp(&newitem->id, workdir_id) == 0);
}