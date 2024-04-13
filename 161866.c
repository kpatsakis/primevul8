static int selinux_proc_get_sid(struct dentry *dentry,
				u16 tclass,
				u32 *sid)
{
	int rc;
	char *buffer, *path;

	buffer = (char *)__get_free_page(GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	path = dentry_path_raw(dentry, buffer, PAGE_SIZE);
	if (IS_ERR(path))
		rc = PTR_ERR(path);
	else {
		/* each process gets a /proc/PID/ entry. Strip off the
		 * PID part to get a valid selinux labeling.
		 * e.g. /proc/1/net/rpc/nfs -> /net/rpc/nfs */
		while (path[1] >= '0' && path[1] <= '9') {
			path[1] = '/';
			path++;
		}
		rc = security_genfs_sid("proc", path, tclass, sid);
	}
	free_page((unsigned long)buffer);
	return rc;
}