check_conffile_perm(const char *username, const char *conf_file)
{
	struct stat stbuf;
	struct passwd *pw = NULL;
	FILE *fp = fopen(conf_file, "r");

	if (fp == NULL) {
		flog(LOG_ERR, "can't open %s: %s", conf_file, strerror(errno));
		return (-1);
	}
	fclose(fp);

	if (!username)
		username = "root";

	pw = getpwnam(username);

	if (stat(conf_file, &stbuf) || pw == NULL)
		return (-1);

	if (stbuf.st_mode & S_IWOTH) {
                flog(LOG_ERR, "Insecure file permissions (writable by others): %s", conf_file);
		return (-1);
        }

	/* for non-root: must not be writable by self/own group */
	if (strncmp(username, "root", 5) != 0 &&
	    ((stbuf.st_mode & S_IWGRP && pw->pw_gid == stbuf.st_gid) ||
	     (stbuf.st_mode & S_IWUSR && pw->pw_uid == stbuf.st_uid))) {
                flog(LOG_ERR, "Insecure file permissions (writable by self/group): %s", conf_file);
		return (-1);
        }

        return 0;
}