isofile_gen_utility_names(struct archive_write *a, struct isofile *file)
{
	struct iso9660 *iso9660;
	const char *pathname;
	char *p, *dirname, *slash;
	size_t len;
	int ret = ARCHIVE_OK;

	iso9660 = a->format_data;

	archive_string_empty(&(file->parentdir));
	archive_string_empty(&(file->basename));
	archive_string_empty(&(file->basename_utf16));
	archive_string_empty(&(file->symlink));

	pathname =  archive_entry_pathname(file->entry);
	if (pathname == NULL || pathname[0] == '\0') {/* virtual root */
		file->dircnt = 0;
		return (ret);
	}

	/*
	 * Make a UTF-16BE basename if Joliet extension enabled.
	 */
	if (iso9660->opt.joliet) {
		const char *u16, *ulast;
		size_t u16len, ulen_last;

		if (iso9660->sconv_to_utf16be == NULL) {
			iso9660->sconv_to_utf16be =
			    archive_string_conversion_to_charset(
				&(a->archive), "UTF-16BE", 1);
			if (iso9660->sconv_to_utf16be == NULL)
				/* Couldn't allocate memory */
				return (ARCHIVE_FATAL);
			iso9660->sconv_from_utf16be =
			    archive_string_conversion_from_charset(
				&(a->archive), "UTF-16BE", 1);
			if (iso9660->sconv_from_utf16be == NULL)
				/* Couldn't allocate memory */
				return (ARCHIVE_FATAL);
		}

		/*
		 * Converte a filename to UTF-16BE.
		 */
		if (0 > archive_entry_pathname_l(file->entry, &u16, &u16len,
		    iso9660->sconv_to_utf16be)) {
			if (errno == ENOMEM) {
				archive_set_error(&a->archive, ENOMEM,
				    "Can't allocate memory for UTF-16BE");
				return (ARCHIVE_FATAL);
			}
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "A filename cannot be converted to UTF-16BE;"
			    "You should disable making Joliet extension");
			ret = ARCHIVE_WARN;
		}

		/*
		 * Make sure a path separator is not in the last;
		 * Remove trailing '/'.
		 */
		while (u16len >= 2) {
#if defined(_WIN32) || defined(__CYGWIN__)
			if (u16[u16len-2] == 0 &&
			    (u16[u16len-1] == '/' || u16[u16len-1] == '\\'))
#else
			if (u16[u16len-2] == 0 && u16[u16len-1] == '/')
#endif
			{
				u16len -= 2;
			} else
				break;
		}

		/*
		 * Find a basename in UTF-16BE.
		 */
		ulast = u16;
		u16len >>= 1;
		ulen_last = u16len;
		while (u16len > 0) {
#if defined(_WIN32) || defined(__CYGWIN__)
			if (u16[0] == 0 && (u16[1] == '/' || u16[1] == '\\'))
#else
			if (u16[0] == 0 && u16[1] == '/')
#endif
			{
				ulast = u16 + 2;
				ulen_last = u16len -1;
			}
			u16 += 2;
			u16len --;
		}
		ulen_last <<= 1;
		if (archive_string_ensure(&(file->basename_utf16),
		    ulen_last) == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate memory for UTF-16BE");
			return (ARCHIVE_FATAL);
		}

		/*
		 * Set UTF-16BE basename.
		 */
		memcpy(file->basename_utf16.s, ulast, ulen_last);
		file->basename_utf16.length = ulen_last;
	}

	archive_strcpy(&(file->parentdir), pathname);
#if defined(_WIN32) || defined(__CYGWIN__)
	/*
	 * Convert a path-separator from '\' to  '/'
	 */
	if (cleanup_backslash_1(file->parentdir.s) != 0) {
		const wchar_t *wp = archive_entry_pathname_w(file->entry);
		struct archive_wstring ws;

		if (wp != NULL) {
			int r;
			archive_string_init(&ws);
			archive_wstrcpy(&ws, wp);
			cleanup_backslash_2(ws.s);
			archive_string_empty(&(file->parentdir));
			r = archive_string_append_from_wcs(&(file->parentdir),
			    ws.s, ws.length);
			archive_wstring_free(&ws);
			if (r < 0 && errno == ENOMEM) {
				archive_set_error(&a->archive, ENOMEM,
				    "Can't allocate memory");
				return (ARCHIVE_FATAL);
			}
		}
	}
#endif

	len = file->parentdir.length;
	p = dirname = file->parentdir.s;

	/*
	 * Remove leading '/', '../' and './' elements
	 */
	while (*p) {
		if (p[0] == '/') {
			p++;
			len--;
		} else if (p[0] != '.')
			break;
		else if (p[1] == '.' && p[2] == '/') {
			p += 3;
			len -= 3;
		} else if (p[1] == '/' || (p[1] == '.' && p[2] == '\0')) {
			p += 2;
			len -= 2;
		} else if (p[1] == '\0') {
			p++;
			len--;
		} else
			break;
	}
	if (p != dirname) {
		memmove(dirname, p, len+1);
		p = dirname;
	}
	/*
	 * Remove "/","/." and "/.." elements from tail.
	 */
	while (len > 0) {
		size_t ll = len;

		if (len > 0 && p[len-1] == '/') {
			p[len-1] = '\0';
			len--;
		}
		if (len > 1 && p[len-2] == '/' && p[len-1] == '.') {
			p[len-2] = '\0';
			len -= 2;
		}
		if (len > 2 && p[len-3] == '/' && p[len-2] == '.' &&
		    p[len-1] == '.') {
			p[len-3] = '\0';
			len -= 3;
		}
		if (ll == len)
			break;
	}
	while (*p) {
		if (p[0] == '/') {
			if (p[1] == '/')
				/* Convert '//' --> '/' */
				strcpy(p, p+1);
			else if (p[1] == '.' && p[2] == '/')
				/* Convert '/./' --> '/' */
				strcpy(p, p+2);
			else if (p[1] == '.' && p[2] == '.' && p[3] == '/') {
				/* Convert 'dir/dir1/../dir2/'
				 *     --> 'dir/dir2/'
				 */
				char *rp = p -1;
				while (rp >= dirname) {
					if (*rp == '/')
						break;
					--rp;
				}
				if (rp > dirname) {
					strcpy(rp, p+3);
					p = rp;
				} else {
					strcpy(dirname, p+4);
					p = dirname;
				}
			} else
				p++;
		} else
			p++;
	}
	p = dirname;
	len = strlen(p);

	if (archive_entry_filetype(file->entry) == AE_IFLNK) {
		/* Convert symlink name too. */
		pathname = archive_entry_symlink(file->entry);
		archive_strcpy(&(file->symlink),  pathname);
#if defined(_WIN32) || defined(__CYGWIN__)
		/*
		 * Convert a path-separator from '\' to  '/'
		 */
		if (archive_strlen(&(file->symlink)) > 0 &&
		    cleanup_backslash_1(file->symlink.s) != 0) {
			const wchar_t *wp =
			    archive_entry_symlink_w(file->entry);
			struct archive_wstring ws;

			if (wp != NULL) {
				int r;
				archive_string_init(&ws);
				archive_wstrcpy(&ws, wp);
				cleanup_backslash_2(ws.s);
				archive_string_empty(&(file->symlink));
				r = archive_string_append_from_wcs(
				    &(file->symlink),
				    ws.s, ws.length);
				archive_wstring_free(&ws);
				if (r < 0 && errno == ENOMEM) {
					archive_set_error(&a->archive, ENOMEM,
					    "Can't allocate memory");
					return (ARCHIVE_FATAL);
				}
			}
		}
#endif
	}
	/*
	 * - Count up directory elements.
	 * - Find out the position which points the last position of
	 *   path separator('/').
	 */
	slash = NULL;
	file->dircnt = 0;
	for (; *p != '\0'; p++)
		if (*p == '/') {
			slash = p;
			file->dircnt++;
		}
	if (slash == NULL) {
		/* The pathname doesn't have a parent directory. */
		file->parentdir.length = len;
		archive_string_copy(&(file->basename), &(file->parentdir));
		archive_string_empty(&(file->parentdir));
		*file->parentdir.s = '\0';
		return (ret);
	}

	/* Make a basename from dirname and slash */
	*slash  = '\0';
	file->parentdir.length = slash - dirname;
	archive_strcpy(&(file->basename),  slash + 1);
	if (archive_entry_filetype(file->entry) == AE_IFDIR)
		file->dircnt ++;
	return (ret);
}