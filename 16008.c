static int do_move_mount_old(struct path *path, const char *old_name)
{
	struct path old_path;
	int err;

	if (!old_name || !*old_name)
		return -EINVAL;

	err = kern_path(old_name, LOOKUP_FOLLOW, &old_path);
	if (err)
		return err;

	err = do_move_mount(&old_path, path);
	path_put(&old_path);
	return err;
}