char *get_devname_from_sysfs(unsigned int major, unsigned int minor)
{
	static char link[256], target[PATH_MAX];
	char *devname;
	ssize_t r;

	snprintf(link, 256, "%s/%u:%u", SYSFS_DEV_BLOCK, major, minor);

	/* Get full path to device knowing its major and minor numbers */
	r = readlink(link, target, PATH_MAX);
	if (r <= 0 || r >= PATH_MAX) {
		return (NULL);
	}

	target[r] = '\0';

	/* Get device name */
	devname = basename(target);
	if (!devname || strnlen(devname, FILENAME_MAX) == 0) {
		return (NULL);
	}

	return (devname);
}