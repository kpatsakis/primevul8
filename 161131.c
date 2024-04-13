isoent_tree(struct archive_write *a, struct isoent **isoentpp)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
	char name[_MAX_FNAME];/* Included null terminator size. */
#elif defined(NAME_MAX) && NAME_MAX >= 255
	char name[NAME_MAX+1];
#else
	char name[256];
#endif
	struct iso9660 *iso9660 = a->format_data;
	struct isoent *dent, *isoent, *np;
	struct isofile *f1, *f2;
	const char *fn, *p;
	int l;

	isoent = *isoentpp;
	dent = iso9660->primary.rootent;
	if (isoent->file->parentdir.length > 0)
		fn = p = isoent->file->parentdir.s;
	else
		fn = p = "";

	/*
	 * If the path of the parent directory of `isoent' entry is
	 * the same as the path of `cur_dirent', add isoent to
	 * `cur_dirent'.
	 */
	if (archive_strlen(&(iso9660->cur_dirstr))
	      == archive_strlen(&(isoent->file->parentdir)) &&
	    strcmp(iso9660->cur_dirstr.s, fn) == 0) {
		if (!isoent_add_child_tail(iso9660->cur_dirent, isoent)) {
			np = (struct isoent *)__archive_rb_tree_find_node(
			    &(iso9660->cur_dirent->rbtree),
			    isoent->file->basename.s);
			goto same_entry;
		}
		return (ARCHIVE_OK);
	}

	for (;;) {
		l = get_path_component(name, sizeof(name), fn);
		if (l == 0) {
			np = NULL;
			break;
		}
		if (l < 0) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_MISC,
			    "A name buffer is too small");
			_isoent_free(isoent);
			return (ARCHIVE_FATAL);
		}

		np = isoent_find_child(dent, name);
		if (np == NULL || fn[0] == '\0')
			break;

		/* Find next subdirectory. */
		if (!np->dir) {
			/* NOT Directory! */
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_MISC,
			    "`%s' is not directory, we cannot insert `%s' ",
			    archive_entry_pathname(np->file->entry),
			    archive_entry_pathname(isoent->file->entry));
			_isoent_free(isoent);
			*isoentpp = NULL;
			return (ARCHIVE_FAILED);
		}
		fn += l;
		if (fn[0] == '/')
			fn++;
		dent = np;
	}
	if (np == NULL) {
		/*
		 * Create virtual parent directories.
		 */
		while (fn[0] != '\0') {
			struct isoent *vp;
			struct archive_string as;

			archive_string_init(&as);
			archive_strncat(&as, p, fn - p + l);
			if (as.s[as.length-1] == '/') {
				as.s[as.length-1] = '\0';
				as.length--;
			}
			vp = isoent_create_virtual_dir(a, iso9660, as.s);
			if (vp == NULL) {
				archive_string_free(&as);
				archive_set_error(&a->archive, ENOMEM,
				    "Can't allocate memory");
				_isoent_free(isoent);
				*isoentpp = NULL;
				return (ARCHIVE_FATAL);
			}
			archive_string_free(&as);

			if (vp->file->dircnt > iso9660->dircnt_max)
				iso9660->dircnt_max = vp->file->dircnt;
			isoent_add_child_tail(dent, vp);
			np = vp;

			fn += l;
			if (fn[0] == '/')
				fn++;
			l = get_path_component(name, sizeof(name), fn);
			if (l < 0) {
				archive_string_free(&as);
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_MISC,
				    "A name buffer is too small");
				_isoent_free(isoent);
				*isoentpp = NULL;
				return (ARCHIVE_FATAL);
			}
			dent = np;
		}

		/* Found out the parent directory where isoent can be
		 * inserted. */
		iso9660->cur_dirent = dent;
		archive_string_empty(&(iso9660->cur_dirstr));
		archive_string_ensure(&(iso9660->cur_dirstr),
		    archive_strlen(&(dent->file->parentdir)) +
		    archive_strlen(&(dent->file->basename)) + 2);
		if (archive_strlen(&(dent->file->parentdir)) +
		    archive_strlen(&(dent->file->basename)) == 0)
			iso9660->cur_dirstr.s[0] = 0;
		else {
			if (archive_strlen(&(dent->file->parentdir)) > 0) {
				archive_string_copy(&(iso9660->cur_dirstr),
				    &(dent->file->parentdir));
				archive_strappend_char(&(iso9660->cur_dirstr), '/');
			}
			archive_string_concat(&(iso9660->cur_dirstr),
			    &(dent->file->basename));
		}

		if (!isoent_add_child_tail(dent, isoent)) {
			np = (struct isoent *)__archive_rb_tree_find_node(
			    &(dent->rbtree), isoent->file->basename.s);
			goto same_entry;
		}
		return (ARCHIVE_OK);
	}

same_entry:
	/*
	 * We have already has the entry the filename of which is
	 * the same.
	 */
	f1 = np->file;
	f2 = isoent->file;

	/* If the file type of entries is different,
	 * we cannot handle it. */
	if (archive_entry_filetype(f1->entry) !=
	    archive_entry_filetype(f2->entry)) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Found duplicate entries `%s' and its file type is "
		    "different",
		    archive_entry_pathname(f1->entry));
		_isoent_free(isoent);
		*isoentpp = NULL;
		return (ARCHIVE_FAILED);
	}

	/* Swap file entries. */
	np->file = f2;
	isoent->file = f1;
	np->virtual = 0;

	_isoent_free(isoent);
	*isoentpp = np;
	return (ARCHIVE_OK);
}