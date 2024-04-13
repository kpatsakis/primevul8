iso9660_close(struct archive_write *a)
{
	struct iso9660 *iso9660;
	int ret, blocks;

	iso9660 = a->format_data;

	/*
	 * Write remaining data out to the temporary file.
	 */
	if (wb_remaining(a) > 0) {
		ret = wb_write_out(a);
		if (ret < 0)
			return (ret);
	}

	/*
	 * Preparations...
	 */
#ifdef DEBUG
	if (iso9660->birth_time == 0)
#endif
		time(&(iso9660->birth_time));

	/*
	 * Prepare a bootable ISO image.
	 */
	if (iso9660->opt.boot) {
		/* Find out the boot file entry. */
		ret = isoent_find_out_boot_file(a, iso9660->primary.rootent);
		if (ret < 0)
			return (ret);
		/* Reconvert the boot file from zisofs'ed form to
		 * plain form. */
		ret = zisofs_rewind_boot_file(a);
		if (ret < 0)
			return (ret);
		/* Write remaining data out to the temporary file. */
		if (wb_remaining(a) > 0) {
			ret = wb_write_out(a);
			if (ret < 0)
				return (ret);
		}
		/* Create the boot catalog. */
		ret = isoent_create_boot_catalog(a, iso9660->primary.rootent);
		if (ret < 0)
			return (ret);
	}

	/*
	 * Prepare joliet extensions.
	 */
	if (iso9660->opt.joliet) {
		/* Make a new tree for joliet. */
		ret = isoent_clone_tree(a, &(iso9660->joliet.rootent),
		    iso9660->primary.rootent);
		if (ret < 0)
			return (ret);
		/* Make sure we have UTF-16BE convertors.
		 * if there is no file entry, convertors are still
		 * uninitilized. */
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
	}

	/*
	 * Make Path Tables.
	 */
	ret = isoent_make_path_table(a);
	if (ret < 0)
		return (ret);

	/*
	 * Calculate a total volume size and setup all locations of
	 * contents of an iso9660 image.
	 */
	blocks = SYSTEM_AREA_BLOCK
		+ PRIMARY_VOLUME_DESCRIPTOR_BLOCK
		+ VOLUME_DESCRIPTOR_SET_TERMINATOR_BLOCK
		+ NON_ISO_FILE_SYSTEM_INFORMATION_BLOCK;
	if (iso9660->opt.boot)
		blocks += BOOT_RECORD_DESCRIPTOR_BLOCK;
	if (iso9660->opt.joliet)
		blocks += SUPPLEMENTARY_VOLUME_DESCRIPTOR_BLOCK;
	if (iso9660->opt.iso_level == 4)
		blocks += SUPPLEMENTARY_VOLUME_DESCRIPTOR_BLOCK;

	/* Setup the locations of Path Table. */
	iso9660->primary.location_type_L_path_table = blocks;
	blocks += iso9660->primary.path_table_block;
	iso9660->primary.location_type_M_path_table = blocks;
	blocks += iso9660->primary.path_table_block;
	if (iso9660->opt.joliet) {
		iso9660->joliet.location_type_L_path_table = blocks;
		blocks += iso9660->joliet.path_table_block;
		iso9660->joliet.location_type_M_path_table = blocks;
		blocks += iso9660->joliet.path_table_block;
	}

	/* Setup the locations of directories. */
	isoent_setup_directory_location(iso9660, blocks,
	    &(iso9660->primary));
	blocks += iso9660->primary.total_dir_block;
	if (iso9660->opt.joliet) {
		isoent_setup_directory_location(iso9660, blocks,
		    &(iso9660->joliet));
		blocks += iso9660->joliet.total_dir_block;
	}

	if (iso9660->opt.rr) {
		iso9660->location_rrip_er = blocks;
		blocks += RRIP_ER_BLOCK;
	}

	/* Setup the locations of all file contents. */
 	isoent_setup_file_location(iso9660, blocks);
	blocks += iso9660->total_file_block;
	if (iso9660->opt.boot && iso9660->opt.boot_info_table) {
		ret = setup_boot_information(a);
		if (ret < 0)
			return (ret);
	}

	/* Now we have a total volume size. */
	iso9660->volume_space_size = blocks;
	if (iso9660->opt.pad)
		iso9660->volume_space_size += PADDING_BLOCK;
	iso9660->volume_sequence_number = 1;


	/*
	 * Write an ISO 9660 image.
	 */

	/* Switc to start using wbuff as file buffer. */
	iso9660->wbuff_remaining = wb_buffmax();
	iso9660->wbuff_type = WB_TO_STREAM;
	iso9660->wbuff_offset = 0;
	iso9660->wbuff_written = 0;
	iso9660->wbuff_tail = 0;

	/* Write The System Area */
	ret = write_null(a, SYSTEM_AREA_BLOCK * LOGICAL_BLOCK_SIZE);
	if (ret != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/* Write Primary Volume Descriptor */
	ret = write_VD(a, &(iso9660->primary));
	if (ret != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	if (iso9660->opt.boot) {
		/* Write Boot Record Volume Descriptor */
		ret = write_VD_boot_record(a);
		if (ret != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
	}

	if (iso9660->opt.iso_level == 4) {
		/* Write Enhanced Volume Descriptor */
		iso9660->primary.vdd_type = VDD_ENHANCED;
		ret = write_VD(a, &(iso9660->primary));
		iso9660->primary.vdd_type = VDD_PRIMARY;
		if (ret != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
	}

	if (iso9660->opt.joliet) {
		ret = write_VD(a, &(iso9660->joliet));
		if (ret != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
	}

	/* Write Volume Descriptor Set Terminator */
	ret = write_VD_terminator(a);
	if (ret != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/* Write Non-ISO File System Information */
	ret = write_information_block(a);
	if (ret != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/* Write Type L Path Table */
	ret = write_path_table(a, 0, &(iso9660->primary));
	if (ret != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/* Write Type M Path Table */
	ret = write_path_table(a, 1, &(iso9660->primary));
	if (ret != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	if (iso9660->opt.joliet) {
		/* Write Type L Path Table */
		ret = write_path_table(a, 0, &(iso9660->joliet));
		if (ret != ARCHIVE_OK)
			return (ARCHIVE_FATAL);

		/* Write Type M Path Table */
		ret = write_path_table(a, 1, &(iso9660->joliet));
		if (ret != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
	}

	/* Write Directory Descriptors */
	ret = write_directory_descriptors(a, &(iso9660->primary));
	if (ret != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	if (iso9660->opt.joliet) {
		ret = write_directory_descriptors(a, &(iso9660->joliet));
		if (ret != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
	}

	if (iso9660->opt.rr) {
		/* Write Rockridge ER(Extensions Reference) */
		ret = write_rr_ER(a);
		if (ret != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
	}

	/* Write File Descriptors */
	ret = write_file_descriptors(a);
	if (ret != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/* Write Padding  */
	if (iso9660->opt.pad) {
		ret = write_null(a, PADDING_BLOCK * LOGICAL_BLOCK_SIZE);
		if (ret != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
	}

	if (iso9660->directories_too_deep != NULL) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "%s: Directories too deep.",
		    archive_entry_pathname(
			iso9660->directories_too_deep->file->entry));
		return (ARCHIVE_WARN);
	}

	/* Write remaining data out. */
	ret = wb_write_out(a);

	return (ret);
}