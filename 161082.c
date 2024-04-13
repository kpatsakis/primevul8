write_information_block(struct archive_write *a)
{
	struct iso9660 *iso9660;
	char buf[128];
	const char *v;
	int opt, r;
	struct archive_string info;
	size_t info_size = LOGICAL_BLOCK_SIZE *
			       NON_ISO_FILE_SYSTEM_INFORMATION_BLOCK;

	iso9660 = (struct iso9660 *)a->format_data;
	if (info_size > wb_remaining(a)) {
		r = wb_write_out(a);
		if (r != ARCHIVE_OK)
			return (r);
	}
	archive_string_init(&info);
	if (archive_string_ensure(&info, info_size) == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate memory");
		return (ARCHIVE_FATAL);
	}
	memset(info.s, 0, info_size);
	opt = 0;
#if defined(HAVE__CTIME64_S)
	_ctime64_s(buf, sizeof(buf), &(iso9660->birth_time));
#elif defined(HAVE_CTIME_R)
	ctime_r(&(iso9660->birth_time), buf);
#else
	strncpy(buf, ctime(&(iso9660->birth_time)), sizeof(buf)-1);
	buf[sizeof(buf)-1] = '\0';
#endif
	archive_string_sprintf(&info,
	    "INFO %s%s", buf, archive_version_string());
	if (iso9660->opt.abstract_file != OPT_ABSTRACT_FILE_DEFAULT)
		set_option_info(&info, &opt, "abstract-file",
		    KEY_STR, iso9660->abstract_file_identifier.s);
	if (iso9660->opt.application_id != OPT_APPLICATION_ID_DEFAULT)
		set_option_info(&info, &opt, "application-id",
		    KEY_STR, iso9660->application_identifier.s);
	if (iso9660->opt.allow_vernum != OPT_ALLOW_VERNUM_DEFAULT)
		set_option_info(&info, &opt, "allow-vernum",
		    KEY_FLG, iso9660->opt.allow_vernum);
	if (iso9660->opt.biblio_file != OPT_BIBLIO_FILE_DEFAULT)
		set_option_info(&info, &opt, "biblio-file",
		    KEY_STR, iso9660->bibliographic_file_identifier.s);
	if (iso9660->opt.boot != OPT_BOOT_DEFAULT)
		set_option_info(&info, &opt, "boot",
		    KEY_STR, iso9660->el_torito.boot_filename.s);
	if (iso9660->opt.boot_catalog != OPT_BOOT_CATALOG_DEFAULT)
		set_option_info(&info, &opt, "boot-catalog",
		    KEY_STR, iso9660->el_torito.catalog_filename.s);
	if (iso9660->opt.boot_info_table != OPT_BOOT_INFO_TABLE_DEFAULT)
		set_option_info(&info, &opt, "boot-info-table",
		    KEY_FLG, iso9660->opt.boot_info_table);
	if (iso9660->opt.boot_load_seg != OPT_BOOT_LOAD_SEG_DEFAULT)
		set_option_info(&info, &opt, "boot-load-seg",
		    KEY_HEX, iso9660->el_torito.boot_load_seg);
	if (iso9660->opt.boot_load_size != OPT_BOOT_LOAD_SIZE_DEFAULT)
		set_option_info(&info, &opt, "boot-load-size",
		    KEY_INT, iso9660->el_torito.boot_load_size);
	if (iso9660->opt.boot_type != OPT_BOOT_TYPE_DEFAULT) {
		v = "no-emulation";
		if (iso9660->opt.boot_type == OPT_BOOT_TYPE_FD)
			v = "fd";
		if (iso9660->opt.boot_type == OPT_BOOT_TYPE_HARD_DISK)
			v = "hard-disk";
		set_option_info(&info, &opt, "boot-type",
		    KEY_STR, v);
	}
#ifdef HAVE_ZLIB_H
	if (iso9660->opt.compression_level != OPT_COMPRESSION_LEVEL_DEFAULT)
		set_option_info(&info, &opt, "compression-level",
		    KEY_INT, iso9660->zisofs.compression_level);
#endif
	if (iso9660->opt.copyright_file != OPT_COPYRIGHT_FILE_DEFAULT)
		set_option_info(&info, &opt, "copyright-file",
		    KEY_STR, iso9660->copyright_file_identifier.s);
	if (iso9660->opt.iso_level != OPT_ISO_LEVEL_DEFAULT)
		set_option_info(&info, &opt, "iso-level",
		    KEY_INT, iso9660->opt.iso_level);
	if (iso9660->opt.joliet != OPT_JOLIET_DEFAULT) {
		if (iso9660->opt.joliet == OPT_JOLIET_LONGNAME)
			set_option_info(&info, &opt, "joliet",
			    KEY_STR, "long");
		else
			set_option_info(&info, &opt, "joliet",
			    KEY_FLG, iso9660->opt.joliet);
	}
	if (iso9660->opt.limit_depth != OPT_LIMIT_DEPTH_DEFAULT)
		set_option_info(&info, &opt, "limit-depth",
		    KEY_FLG, iso9660->opt.limit_depth);
	if (iso9660->opt.limit_dirs != OPT_LIMIT_DIRS_DEFAULT)
		set_option_info(&info, &opt, "limit-dirs",
		    KEY_FLG, iso9660->opt.limit_dirs);
	if (iso9660->opt.pad != OPT_PAD_DEFAULT)
		set_option_info(&info, &opt, "pad",
		    KEY_FLG, iso9660->opt.pad);
	if (iso9660->opt.publisher != OPT_PUBLISHER_DEFAULT)
		set_option_info(&info, &opt, "publisher",
		    KEY_STR, iso9660->publisher_identifier.s);
	if (iso9660->opt.rr != OPT_RR_DEFAULT) {
		if (iso9660->opt.rr == OPT_RR_DISABLED)
			set_option_info(&info, &opt, "rockridge",
			    KEY_FLG, iso9660->opt.rr);
		else if (iso9660->opt.rr == OPT_RR_STRICT)
			set_option_info(&info, &opt, "rockridge",
			    KEY_STR, "strict");
		else if (iso9660->opt.rr == OPT_RR_USEFUL)
			set_option_info(&info, &opt, "rockridge",
			    KEY_STR, "useful");
	}
	if (iso9660->opt.volume_id != OPT_VOLUME_ID_DEFAULT)
		set_option_info(&info, &opt, "volume-id",
		    KEY_STR, iso9660->volume_identifier.s);
	if (iso9660->opt.zisofs != OPT_ZISOFS_DEFAULT)
		set_option_info(&info, &opt, "zisofs",
		    KEY_FLG, iso9660->opt.zisofs);

	memcpy(wb_buffptr(a), info.s, info_size);
	archive_string_free(&info);
	return (wb_consume(a, info_size));
}