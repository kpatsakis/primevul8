static char *copy_mount_string(const void __user *data)
{
	return data ? strndup_user(data, PATH_MAX) : NULL;
}