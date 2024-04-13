char *get_devname(unsigned int major, unsigned int minor, int pretty)
{
	static char buf[32];
	char *name;

	snprintf(buf, 32, "dev%u-%u", major, minor);

	if (!pretty)
		return (buf);

	name = get_devname_from_sysfs(major, minor);
	if (name != NULL)
		return (name);

	name = ioc_name(major, minor);
	if ((name != NULL) && strcmp(name, K_NODEV))
		return (name);

	return (buf);
}