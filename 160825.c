static char *kvp_get_if_name(char *guid)
{
	DIR *dir;
	struct dirent *entry;
	FILE    *file;
	char    *p, *q, *x;
	char    *if_name = NULL;
	char    buf[256];
	char *kvp_net_dir = "/sys/class/net/";
	char dev_id[256];

	dir = opendir(kvp_net_dir);
	if (dir == NULL)
		return NULL;

	snprintf(dev_id, sizeof(dev_id), "%s", kvp_net_dir);
	q = dev_id + strlen(kvp_net_dir);

	while ((entry = readdir(dir)) != NULL) {
		/*
		 * Set the state for the next pass.
		 */
		*q = '\0';
		strcat(dev_id, entry->d_name);
		strcat(dev_id, "/device/device_id");

		file = fopen(dev_id, "r");
		if (file == NULL)
			continue;

		p = fgets(buf, sizeof(buf), file);
		if (p) {
			x = strchr(p, '\n');
			if (x)
				*x = '\0';

			if (!strcmp(p, guid)) {
				/*
				 * Found the guid match; return the interface
				 * name. The caller will free the memory.
				 */
				if_name = strdup(entry->d_name);
				fclose(file);
				break;
			}
		}
		fclose(file);
	}

	closedir(dir);
	return if_name;
}