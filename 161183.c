archive_write_set_format_iso9660(struct archive *_a)
{
	struct archive_write *a = (struct archive_write *)_a;
	struct iso9660 *iso9660;

	archive_check_magic(_a, ARCHIVE_WRITE_MAGIC,
	    ARCHIVE_STATE_NEW, "archive_write_set_format_iso9660");

	/* If another format was already registered, unregister it. */
	if (a->format_free != NULL)
		(a->format_free)(a);

	iso9660 = calloc(1, sizeof(*iso9660));
	if (iso9660 == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate iso9660 data");
		return (ARCHIVE_FATAL);
	}
	iso9660->birth_time = 0;
	iso9660->temp_fd = -1;
	iso9660->cur_file = NULL;
	iso9660->primary.max_depth = 0;
	iso9660->primary.vdd_type = VDD_PRIMARY;
	iso9660->primary.pathtbl = NULL;
	iso9660->joliet.rootent = NULL;
	iso9660->joliet.max_depth = 0;
	iso9660->joliet.vdd_type = VDD_JOLIET;
	iso9660->joliet.pathtbl = NULL;
	isofile_init_entry_list(iso9660);
	isofile_init_entry_data_file_list(iso9660);
	isofile_init_hardlinks(iso9660);
	iso9660->directories_too_deep = NULL;
	iso9660->dircnt_max = 1;
	iso9660->wbuff_remaining = wb_buffmax();
	iso9660->wbuff_type = WB_TO_TEMP;
	iso9660->wbuff_offset = 0;
	iso9660->wbuff_written = 0;
	iso9660->wbuff_tail = 0;
	archive_string_init(&(iso9660->utf16be));
	archive_string_init(&(iso9660->mbs));

	/*
	 * Init Identifiers used for PVD and SVD.
	 */
	archive_string_init(&(iso9660->volume_identifier));
	archive_strcpy(&(iso9660->volume_identifier), "CDROM");
	archive_string_init(&(iso9660->publisher_identifier));
	archive_string_init(&(iso9660->data_preparer_identifier));
	archive_string_init(&(iso9660->application_identifier));
	archive_strcpy(&(iso9660->application_identifier),
	    archive_version_string());
	archive_string_init(&(iso9660->copyright_file_identifier));
	archive_string_init(&(iso9660->abstract_file_identifier));
	archive_string_init(&(iso9660->bibliographic_file_identifier));

	/*
	 * Init El Torito bootable CD variables.
	 */
	archive_string_init(&(iso9660->el_torito.catalog_filename));
	iso9660->el_torito.catalog = NULL;
	/* Set default file name of boot catalog  */
	archive_strcpy(&(iso9660->el_torito.catalog_filename),
	    "boot.catalog");
	archive_string_init(&(iso9660->el_torito.boot_filename));
	iso9660->el_torito.boot = NULL;
	iso9660->el_torito.platform_id = BOOT_PLATFORM_X86;
	archive_string_init(&(iso9660->el_torito.id));
	iso9660->el_torito.boot_load_seg = 0;
	iso9660->el_torito.boot_load_size = BOOT_LOAD_SIZE;

	/*
	 * Init zisofs variables.
	 */
#ifdef HAVE_ZLIB_H
	iso9660->zisofs.block_pointers = NULL;
	iso9660->zisofs.block_pointers_allocated = 0;
	iso9660->zisofs.stream_valid = 0;
	iso9660->zisofs.compression_level = 9;
	memset(&(iso9660->zisofs.stream), 0,
	    sizeof(iso9660->zisofs.stream));
#endif

	/*
	 * Set default value of iso9660 options.
	 */
	iso9660->opt.abstract_file = OPT_ABSTRACT_FILE_DEFAULT;
	iso9660->opt.application_id = OPT_APPLICATION_ID_DEFAULT;
	iso9660->opt.allow_vernum = OPT_ALLOW_VERNUM_DEFAULT;
	iso9660->opt.biblio_file = OPT_BIBLIO_FILE_DEFAULT;
	iso9660->opt.boot = OPT_BOOT_DEFAULT;
	iso9660->opt.boot_catalog = OPT_BOOT_CATALOG_DEFAULT;
	iso9660->opt.boot_info_table = OPT_BOOT_INFO_TABLE_DEFAULT;
	iso9660->opt.boot_load_seg = OPT_BOOT_LOAD_SEG_DEFAULT;
	iso9660->opt.boot_load_size = OPT_BOOT_LOAD_SIZE_DEFAULT;
	iso9660->opt.boot_type = OPT_BOOT_TYPE_DEFAULT;
	iso9660->opt.compression_level = OPT_COMPRESSION_LEVEL_DEFAULT;
	iso9660->opt.copyright_file = OPT_COPYRIGHT_FILE_DEFAULT;
	iso9660->opt.iso_level = OPT_ISO_LEVEL_DEFAULT;
	iso9660->opt.joliet = OPT_JOLIET_DEFAULT;
	iso9660->opt.limit_depth = OPT_LIMIT_DEPTH_DEFAULT;
	iso9660->opt.limit_dirs = OPT_LIMIT_DIRS_DEFAULT;
	iso9660->opt.pad = OPT_PAD_DEFAULT;
	iso9660->opt.publisher = OPT_PUBLISHER_DEFAULT;
	iso9660->opt.rr = OPT_RR_DEFAULT;
	iso9660->opt.volume_id = OPT_VOLUME_ID_DEFAULT;
	iso9660->opt.zisofs = OPT_ZISOFS_DEFAULT;

	/* Create the root directory. */
	iso9660->primary.rootent =
	    isoent_create_virtual_dir(a, iso9660, "");
	if (iso9660->primary.rootent == NULL) {
		free(iso9660);
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate memory");
		return (ARCHIVE_FATAL);
	}
	iso9660->primary.rootent->parent = iso9660->primary.rootent;
	iso9660->cur_dirent = iso9660->primary.rootent;
	archive_string_init(&(iso9660->cur_dirstr));
	archive_string_ensure(&(iso9660->cur_dirstr), 1);
	iso9660->cur_dirstr.s[0] = 0;
	iso9660->sconv_to_utf16be = NULL;
	iso9660->sconv_from_utf16be = NULL;

	a->format_data = iso9660;
	a->format_name = "iso9660";
	a->format_options = iso9660_options;
	a->format_write_header = iso9660_write_header;
	a->format_write_data = iso9660_write_data;
	a->format_finish_entry = iso9660_finish_entry;
	a->format_close = iso9660_close;
	a->format_free = iso9660_free;
	a->archive.archive_format = ARCHIVE_FORMAT_ISO9660;
	a->archive.archive_format_name = "ISO9660";

	return (ARCHIVE_OK);
}