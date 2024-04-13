archive_read_format_lha_read_header(struct archive_read *a,
    struct archive_entry *entry)
{
	struct archive_wstring linkname;
	struct archive_wstring pathname;
	struct lha *lha;
	const unsigned char *p;
	const char *signature;
	int err;
	struct archive_mstring conv_buffer;
	const wchar_t *conv_buffer_p;

	lha_crc16_init();

	a->archive.archive_format = ARCHIVE_FORMAT_LHA;
	if (a->archive.archive_format_name == NULL)
		a->archive.archive_format_name = "lha";

	lha = (struct lha *)(a->format->data);
	lha->decompress_init = 0;
	lha->end_of_entry = 0;
	lha->end_of_entry_cleanup = 0;
	lha->entry_unconsumed = 0;

	if ((p = __archive_read_ahead(a, H_SIZE, NULL)) == NULL) {
		/*
		 * LHa archiver added 0 to the tail of its archive file as
		 * the mark of the end of the archive.
		 */
		signature = __archive_read_ahead(a, sizeof(signature[0]), NULL);
		if (signature == NULL || signature[0] == 0)
			return (ARCHIVE_EOF);
		return (truncated_error(a));
	}

	signature = (const char *)p;
	if (lha->found_first_header == 0 &&
	    signature[0] == 'M' && signature[1] == 'Z') {
                /* This is an executable?  Must be self-extracting... 	*/
		err = lha_skip_sfx(a);
		if (err < ARCHIVE_WARN)
			return (err);

		if ((p = __archive_read_ahead(a, sizeof(*p), NULL)) == NULL)
			return (truncated_error(a));
		signature = (const char *)p;
	}
	/* signature[0] == 0 means the end of an LHa archive file. */
	if (signature[0] == 0)
		return (ARCHIVE_EOF);

	/*
	 * Check the header format and method type.
	 */
	if (lha_check_header_format(p) != 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Bad LHa file");
		return (ARCHIVE_FATAL);
	}

	/* We've found the first header. */
	lha->found_first_header = 1;
	/* Set a default value and common data */
	lha->header_size = 0;
	lha->level = p[H_LEVEL_OFFSET];
	lha->method[0] = p[H_METHOD_OFFSET+1];
	lha->method[1] = p[H_METHOD_OFFSET+2];
	lha->method[2] = p[H_METHOD_OFFSET+3];
	if (memcmp(lha->method, "lhd", 3) == 0)
		lha->directory = 1;
	else
		lha->directory = 0;
	if (memcmp(lha->method, "lh0", 3) == 0 ||
	    memcmp(lha->method, "lz4", 3) == 0)
		lha->entry_is_compressed = 0;
	else
		lha->entry_is_compressed = 1;

	lha->compsize = 0;
	lha->origsize = 0;
	lha->setflag = 0;
	lha->birthtime = 0;
	lha->birthtime_tv_nsec = 0;
	lha->mtime = 0;
	lha->mtime_tv_nsec = 0;
	lha->atime = 0;
	lha->atime_tv_nsec = 0;
	lha->mode = (lha->directory)? 0777 : 0666;
	lha->uid = 0;
	lha->gid = 0;
	archive_string_empty(&lha->dirname);
	archive_string_empty(&lha->filename);
	lha->dos_attr = 0;
	if (lha->opt_sconv != NULL) {
		lha->sconv_dir = lha->opt_sconv;
		lha->sconv_fname = lha->opt_sconv;
	} else {
		lha->sconv_dir = NULL;
		lha->sconv_fname = NULL;
	}

	switch (p[H_LEVEL_OFFSET]) {
	case 0:
		err = lha_read_file_header_0(a, lha);
		break;
	case 1:
		err = lha_read_file_header_1(a, lha);
		break;
	case 2:
		err = lha_read_file_header_2(a, lha);
		break;
	case 3:
		err = lha_read_file_header_3(a, lha);
		break;
	default:
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Unsupported LHa header level %d", p[H_LEVEL_OFFSET]);
		err = ARCHIVE_FATAL;
		break;
	}
	if (err < ARCHIVE_WARN)
		return (err);


	if (!lha->directory && archive_strlen(&lha->filename) == 0)
		/* The filename has not been set */
		return (truncated_error(a));

	/*
	 * Make a pathname from a dirname and a filename, after converting to Unicode.
	 * This is because codepages might differ between dirname and filename.
	*/
	archive_string_init(&pathname);
	archive_string_init(&linkname);
	archive_string_init(&conv_buffer.aes_mbs);
	archive_string_init(&conv_buffer.aes_mbs_in_locale);
	archive_string_init(&conv_buffer.aes_utf8);
	archive_string_init(&conv_buffer.aes_wcs);
	if (0 != archive_mstring_copy_mbs_len_l(&conv_buffer, lha->dirname.s, lha->dirname.length, lha->sconv_dir)) {
		archive_set_error(&a->archive,
			ARCHIVE_ERRNO_FILE_FORMAT,
			"Pathname cannot be converted "
			"from %s to Unicode.",
			archive_string_conversion_charset_name(lha->sconv_dir));
		return ARCHIVE_FATAL;
	}
	if (0 != archive_mstring_get_wcs(&a->archive, &conv_buffer, &conv_buffer_p)) {
		archive_mstring_clean(&conv_buffer);
		archive_wstring_free(&pathname);
		archive_wstring_free(&linkname);
		return ARCHIVE_FATAL;
	}
	archive_wstring_copy(&pathname, &conv_buffer.aes_wcs);

	archive_string_empty(&conv_buffer.aes_mbs);
	archive_string_empty(&conv_buffer.aes_mbs_in_locale);
	archive_string_empty(&conv_buffer.aes_utf8);
	archive_wstring_empty(&conv_buffer.aes_wcs);
	if (0 != archive_mstring_copy_mbs_len_l(&conv_buffer, lha->filename.s, lha->filename.length, lha->sconv_fname)) {
		archive_set_error(&a->archive,
			ARCHIVE_ERRNO_FILE_FORMAT,
			"Pathname cannot be converted "
			"from %s to Unicode.",
			archive_string_conversion_charset_name(lha->sconv_fname));
		return ARCHIVE_FATAL;
	}
	if (0 != archive_mstring_get_wcs(&a->archive, &conv_buffer, &conv_buffer_p)) {
		archive_mstring_clean(&conv_buffer);
		archive_wstring_free(&pathname);
		archive_wstring_free(&linkname);
		return ARCHIVE_FATAL;
	}
	archive_wstring_concat(&pathname, &conv_buffer.aes_wcs);
	archive_mstring_clean(&conv_buffer);

	if ((lha->mode & AE_IFMT) == AE_IFLNK) {
		/*
	 	 * Extract the symlink-name if it's included in the pathname.
	 	 */
		if (!lha_parse_linkname(&linkname, &pathname)) {
			/* We couldn't get the symlink-name. */
			archive_set_error(&a->archive,
		    	    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Unknown symlink-name");
			archive_wstring_free(&pathname);
			archive_wstring_free(&linkname);
			return (ARCHIVE_FAILED);
		}
	} else {
		/*
		 * Make sure a file-type is set.
		 * The mode has been overridden if it is in the extended data.
		 */
		lha->mode = (lha->mode & ~AE_IFMT) |
		    ((lha->directory)? AE_IFDIR: AE_IFREG);
	}
	if ((lha->setflag & UNIX_MODE_IS_SET) == 0 &&
	    (lha->dos_attr & 1) != 0)
		lha->mode &= ~(0222);/* read only. */

	/*
	 * Set basic file parameters.
	 */
	archive_entry_copy_pathname_w(entry, pathname.s);
	archive_wstring_free(&pathname);
	if (archive_strlen(&linkname) > 0) {
		archive_entry_copy_symlink_w(entry, linkname.s);
	} else
		archive_entry_set_symlink(entry, NULL);
	archive_wstring_free(&linkname);
	/*
	 * When a header level is 0, there is a possibility that
	 * a pathname and a symlink has '\' character, a directory
	 * separator in DOS/Windows. So we should convert it to '/'.
	 */
	if (p[H_LEVEL_OFFSET] == 0)
		lha_replace_path_separator(lha, entry);

	archive_entry_set_mode(entry, lha->mode);
	archive_entry_set_uid(entry, lha->uid);
	archive_entry_set_gid(entry, lha->gid);
	if (archive_strlen(&lha->uname) > 0)
		archive_entry_set_uname(entry, lha->uname.s);
	if (archive_strlen(&lha->gname) > 0)
		archive_entry_set_gname(entry, lha->gname.s);
	if (lha->setflag & BIRTHTIME_IS_SET) {
		archive_entry_set_birthtime(entry, lha->birthtime,
		    lha->birthtime_tv_nsec);
		archive_entry_set_ctime(entry, lha->birthtime,
		    lha->birthtime_tv_nsec);
	} else {
		archive_entry_unset_birthtime(entry);
		archive_entry_unset_ctime(entry);
	}
	archive_entry_set_mtime(entry, lha->mtime, lha->mtime_tv_nsec);
	if (lha->setflag & ATIME_IS_SET)
		archive_entry_set_atime(entry, lha->atime,
		    lha->atime_tv_nsec);
	else
		archive_entry_unset_atime(entry);
	if (lha->directory || archive_entry_symlink(entry) != NULL)
		archive_entry_unset_size(entry);
	else
		archive_entry_set_size(entry, lha->origsize);

	/*
	 * Prepare variables used to read a file content.
	 */
	lha->entry_bytes_remaining = lha->compsize;
	if (lha->entry_bytes_remaining < 0) {
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_FILE_FORMAT,
		    "Invalid LHa entry size");
		return (ARCHIVE_FATAL);
	}
	lha->entry_offset = 0;
	lha->entry_crc_calculated = 0;

	/*
	 * This file does not have a content.
	 */
	if (lha->directory || lha->compsize == 0)
		lha->end_of_entry = 1;

	sprintf(lha->format_name, "lha -%c%c%c-",
	    lha->method[0], lha->method[1], lha->method[2]);
	a->archive.archive_format_name = lha->format_name;

	return (err);
}