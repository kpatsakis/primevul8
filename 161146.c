iso9660_write_header(struct archive_write *a, struct archive_entry *entry)
{
	struct iso9660 *iso9660;
	struct isofile *file;
	struct isoent *isoent;
	int r, ret = ARCHIVE_OK;

	iso9660 = a->format_data;

	iso9660->cur_file = NULL;
	iso9660->bytes_remaining = 0;
	iso9660->need_multi_extent = 0;
	if (archive_entry_filetype(entry) == AE_IFLNK
	    && iso9660->opt.rr == OPT_RR_DISABLED) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Ignore symlink file.");
		iso9660->cur_file = NULL;
		return (ARCHIVE_WARN);
	}
	if (archive_entry_filetype(entry) == AE_IFREG &&
	    archive_entry_size(entry) >= MULTI_EXTENT_SIZE) {
		if (iso9660->opt.iso_level < 3) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_MISC,
			    "Ignore over %lld bytes file. "
			    "This file too large.",
			    MULTI_EXTENT_SIZE);
				iso9660->cur_file = NULL;
			return (ARCHIVE_WARN);
		}
		iso9660->need_multi_extent = 1;
	}

	file = isofile_new(a, entry);
	if (file == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate data");
		return (ARCHIVE_FATAL);
	}
	r = isofile_gen_utility_names(a, file);
	if (r < ARCHIVE_WARN) {
		isofile_free(file);
		return (r);
	}
	else if (r < ret)
		ret = r;

	/*
	 * Ignore a path which looks like the top of directory name
	 * since we have already made the root directory of an ISO image.
	 */
	if (archive_strlen(&(file->parentdir)) == 0 &&
	    archive_strlen(&(file->basename)) == 0) {
		isofile_free(file);
		return (r);
	}

	isofile_add_entry(iso9660, file);
	isoent = isoent_new(file);
	if (isoent == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate data");
		return (ARCHIVE_FATAL);
	}
	if (isoent->file->dircnt > iso9660->dircnt_max)
		iso9660->dircnt_max = isoent->file->dircnt;

	/* Add the current file into tree */
	r = isoent_tree(a, &isoent);
	if (r != ARCHIVE_OK)
		return (r);

	/* If there is the same file in tree and
	 * the current file is older than the file in tree.
	 * So we don't need the current file data anymore. */
	if (isoent->file != file)
		return (ARCHIVE_OK);

	/* Non regular files contents are unneeded to be saved to
	 * temporary files. */
	if (archive_entry_filetype(file->entry) != AE_IFREG)
		return (ret);

	/*
	 * Set the current file to cur_file to read its contents.
	 */
	iso9660->cur_file = file;

	if (archive_entry_nlink(file->entry) > 1) {
		r = isofile_register_hardlink(a, file);
		if (r != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
	}

	/*
	 * Prepare to save the contents of the file.
	 */
	if (iso9660->temp_fd < 0) {
		iso9660->temp_fd = __archive_mktemp(NULL);
		if (iso9660->temp_fd < 0) {
			archive_set_error(&a->archive, errno,
			    "Couldn't create temporary file");
			return (ARCHIVE_FATAL);
		}
	}

	/* Save an offset of current file in temporary file. */
	file->content.offset_of_temp = wb_offset(a);
	file->cur_content = &(file->content);
	r = zisofs_init(a, file);
	if (r < ret)
		ret = r;
	iso9660->bytes_remaining =  archive_entry_size(file->entry);

	return (ret);
}