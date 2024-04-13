char *get_sa_devname(unsigned int major, unsigned int minor, unsigned int flags)
{
	char *dev_name = NULL, *persist_dev_name = NULL;

	if (DISPLAY_PERSIST_NAME_S(flags)) {
		persist_dev_name = get_persistent_name_from_pretty(get_devname(major, minor, TRUE));
	}

	if (persist_dev_name) {
		dev_name = persist_dev_name;
	}
	else {
		if ((USE_PRETTY_OPTION(flags)) && (major == dm_major)) {
			dev_name = transform_devmapname(major, minor);
		}

		if (!dev_name) {
			dev_name = get_devname(major, minor,
					       USE_PRETTY_OPTION(flags));
		}
	}

	return dev_name;
}