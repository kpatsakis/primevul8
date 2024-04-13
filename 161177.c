isoent_create_boot_catalog(struct archive_write *a, struct isoent *rootent)
{
	struct iso9660 *iso9660 = a->format_data;
	struct isofile *file;
	struct isoent *isoent;
	struct archive_entry *entry;

	(void)rootent; /* UNUSED */
	/*
	 * Create the entry which is the "boot.catalog" file.
	 */
	file = isofile_new(a, NULL);
	if (file == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate memory");
		return (ARCHIVE_FATAL);
	}
	archive_entry_set_pathname(file->entry,
	    iso9660->el_torito.catalog_filename.s);
	archive_entry_set_size(file->entry, LOGICAL_BLOCK_SIZE);
	archive_entry_set_mtime(file->entry, iso9660->birth_time, 0);
	archive_entry_set_atime(file->entry, iso9660->birth_time, 0);
	archive_entry_set_ctime(file->entry, iso9660->birth_time, 0);
	archive_entry_set_uid(file->entry, getuid());
	archive_entry_set_gid(file->entry, getgid());
	archive_entry_set_mode(file->entry, AE_IFREG | 0444);
	archive_entry_set_nlink(file->entry, 1);

	if (isofile_gen_utility_names(a, file) < ARCHIVE_WARN) {
		isofile_free(file);
		return (ARCHIVE_FATAL);
	}
	file->boot = BOOT_CATALOG;
	file->content.size = LOGICAL_BLOCK_SIZE;
	isofile_add_entry(iso9660, file);

	isoent = isoent_new(file);
	if (isoent == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate memory");
		return (ARCHIVE_FATAL);
	}
	isoent->virtual = 1;

	/* Add the "boot.catalog" entry into tree */
	if (isoent_tree(a, &isoent) != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	iso9660->el_torito.catalog = isoent;
	/*
	 * Get a boot medai type.
	 */
	switch (iso9660->opt.boot_type) {
	default:
	case OPT_BOOT_TYPE_AUTO:
		/* Try detecting a media type of the boot image. */
		entry = iso9660->el_torito.boot->file->entry;
		if (archive_entry_size(entry) == FD_1_2M_SIZE)
			iso9660->el_torito.media_type =
			    BOOT_MEDIA_1_2M_DISKETTE;
		else if (archive_entry_size(entry) == FD_1_44M_SIZE)
			iso9660->el_torito.media_type =
			    BOOT_MEDIA_1_44M_DISKETTE;
		else if (archive_entry_size(entry) == FD_2_88M_SIZE)
			iso9660->el_torito.media_type =
			    BOOT_MEDIA_2_88M_DISKETTE;
		else
			/* We cannot decide whether the boot image is
			 * hard-disk. */
			iso9660->el_torito.media_type =
			    BOOT_MEDIA_NO_EMULATION;
		break;
	case OPT_BOOT_TYPE_NO_EMU:
		iso9660->el_torito.media_type = BOOT_MEDIA_NO_EMULATION;
		break;
	case OPT_BOOT_TYPE_HARD_DISK:
		iso9660->el_torito.media_type = BOOT_MEDIA_HARD_DISK;
		break;
	case OPT_BOOT_TYPE_FD:
		entry = iso9660->el_torito.boot->file->entry;
		if (archive_entry_size(entry) <= FD_1_2M_SIZE)
			iso9660->el_torito.media_type =
			    BOOT_MEDIA_1_2M_DISKETTE;
		else if (archive_entry_size(entry) <= FD_1_44M_SIZE)
			iso9660->el_torito.media_type =
			    BOOT_MEDIA_1_44M_DISKETTE;
		else if (archive_entry_size(entry) <= FD_2_88M_SIZE)
			iso9660->el_torito.media_type =
			    BOOT_MEDIA_2_88M_DISKETTE;
		else {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Boot image file(``%s'') size is too big "
			    "for fd type.",
			    iso9660->el_torito.boot_filename.s);
			return (ARCHIVE_FATAL);
		}
		break;
	}

	/*
	 * Get a system type.
	 * TODO: `El Torito' specification says "A copy of byte 5 from the
	 *       Partition Table found in the boot image".
	 */
	iso9660->el_torito.system_type = 0;

	/*
	 * Get an ID.
	 */
	if (iso9660->opt.publisher)
		archive_string_copy(&(iso9660->el_torito.id),
		    &(iso9660->publisher_identifier));


	return (ARCHIVE_OK);
}