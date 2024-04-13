void kvp_get_os_info(void)
{
	FILE	*file;
	char	*p, buf[512];

	uname(&uts_buf);
	os_version = uts_buf.release;
	os_build = strdup(uts_buf.release);

	os_name = uts_buf.sysname;
	processor_arch = uts_buf.machine;

	/*
	 * The current windows host (win7) expects the build
	 * string to be of the form: x.y.z
	 * Strip additional information we may have.
	 */
	p = strchr(os_version, '-');
	if (p)
		*p = '\0';

	/*
	 * Parse the /etc/os-release file if present:
	 * http://www.freedesktop.org/software/systemd/man/os-release.html
	 */
	file = fopen("/etc/os-release", "r");
	if (file != NULL) {
		while (fgets(buf, sizeof(buf), file)) {
			char *value, *q;

			/* Ignore comments */
			if (buf[0] == '#')
				continue;

			/* Split into name=value */
			p = strchr(buf, '=');
			if (!p)
				continue;
			*p++ = 0;

			/* Remove quotes and newline; un-escape */
			value = p;
			q = p;
			while (*p) {
				if (*p == '\\') {
					++p;
					if (!*p)
						break;
					*q++ = *p++;
				} else if (*p == '\'' || *p == '"' ||
					   *p == '\n') {
					++p;
				} else {
					*q++ = *p++;
				}
			}
			*q = 0;

			if (!strcmp(buf, "NAME")) {
				p = strdup(value);
				if (!p)
					break;
				os_name = p;
			} else if (!strcmp(buf, "VERSION_ID")) {
				p = strdup(value);
				if (!p)
					break;
				os_major = p;
			}
		}
		fclose(file);
		return;
	}

	/* Fallback for older RH/SUSE releases */
	file = fopen("/etc/SuSE-release", "r");
	if (file != NULL)
		goto kvp_osinfo_found;
	file  = fopen("/etc/redhat-release", "r");
	if (file != NULL)
		goto kvp_osinfo_found;

	/*
	 * We don't have information about the os.
	 */
	return;

kvp_osinfo_found:
	/* up to three lines */
	p = fgets(buf, sizeof(buf), file);
	if (p) {
		p = strchr(buf, '\n');
		if (p)
			*p = '\0';
		p = strdup(buf);
		if (!p)
			goto done;
		os_name = p;

		/* second line */
		p = fgets(buf, sizeof(buf), file);
		if (p) {
			p = strchr(buf, '\n');
			if (p)
				*p = '\0';
			p = strdup(buf);
			if (!p)
				goto done;
			os_major = p;

			/* third line */
			p = fgets(buf, sizeof(buf), file);
			if (p)  {
				p = strchr(buf, '\n');
				if (p)
					*p = '\0';
				p = strdup(buf);
				if (p)
					os_minor = p;
			}
		}
	}

done:
	fclose(file);
	return;
}