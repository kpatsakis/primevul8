write_VD(struct archive_write *a, struct vdd *vdd)
{
	struct iso9660 *iso9660;
	unsigned char *bp;
	uint16_t volume_set_size = 1;
	char identifier[256];
	enum VD_type vdt;
	enum vdc vdc;
	unsigned char vd_ver, fst_ver;
	int r;

	iso9660 = a->format_data;
	switch (vdd->vdd_type) {
	case VDD_JOLIET:
		vdt = VDT_SUPPLEMENTARY;
		vd_ver = fst_ver = 1;
		vdc = VDC_UCS2;
		break;
	case VDD_ENHANCED:
		vdt = VDT_SUPPLEMENTARY;
		vd_ver = fst_ver = 2;
		vdc = VDC_LOWERCASE;
		break;
	case VDD_PRIMARY:
	default:
		vdt = VDT_PRIMARY;
		vd_ver = fst_ver = 1;
#ifdef COMPAT_MKISOFS
		vdc = VDC_LOWERCASE;
#else
		vdc = VDC_STD;
#endif
		break;
	}

	bp = wb_buffptr(a) -1;
	/* Volume Descriptor Type */
	set_VD_bp(bp, vdt, vd_ver);
	/* Unused Field */
	set_unused_field_bp(bp, 8, 8);
	/* System Identifier */
	get_system_identitier(identifier, sizeof(identifier));
	r = set_str_a_characters_bp(a, bp, 9, 40, identifier, vdc);
	if (r != ARCHIVE_OK)
		return (r);
	/* Volume Identifier */
	r = set_str_d_characters_bp(a, bp, 41, 72,
	    iso9660->volume_identifier.s, vdc);
	if (r != ARCHIVE_OK)
		return (r);
	/* Unused Field */
	set_unused_field_bp(bp, 73, 80);
	/* Volume Space Size */
	set_num_733(bp+81, iso9660->volume_space_size);
	if (vdd->vdd_type == VDD_JOLIET) {
		/* Escape Sequences */
		bp[89] = 0x25;/* UCS-2 Level 3 */
		bp[90] = 0x2F;
		bp[91] = 0x45;
		memset(bp + 92, 0, 120 - 92 + 1);
	} else {
		/* Unused Field */
		set_unused_field_bp(bp, 89, 120);
	}
	/* Volume Set Size */
	set_num_723(bp+121, volume_set_size);
	/* Volume Sequence Number */
	set_num_723(bp+125, iso9660->volume_sequence_number);
	/* Logical Block Size */
	set_num_723(bp+129, LOGICAL_BLOCK_SIZE);
	/* Path Table Size */
	set_num_733(bp+133, vdd->path_table_size);
	/* Location of Occurrence of Type L Path Table */
	set_num_731(bp+141, vdd->location_type_L_path_table);
	/* Location of Optional Occurrence of Type L Path Table */
	set_num_731(bp+145, 0);
	/* Location of Occurrence of Type M Path Table */
	set_num_732(bp+149, vdd->location_type_M_path_table);
	/* Location of Optional Occurrence of Type M Path Table */
	set_num_732(bp+153, 0);
	/* Directory Record for Root Directory(BP 157 to 190) */
	set_directory_record(bp+157, 190-157+1, vdd->rootent,
	    iso9660, DIR_REC_VD, vdd->vdd_type);
	/* Volume Set Identifier */
	r = set_str_d_characters_bp(a, bp, 191, 318, "", vdc);
	if (r != ARCHIVE_OK)
		return (r);
	/* Publisher Identifier */
	r = set_file_identifier(bp, 319, 446, vdc, a, vdd,
	    &(iso9660->publisher_identifier),
	    "Publisher File", 1, A_CHAR);
	if (r != ARCHIVE_OK)
		return (r);
	/* Data Preparer Identifier */
	r = set_file_identifier(bp, 447, 574, vdc, a, vdd,
	    &(iso9660->data_preparer_identifier),
	    "Data Preparer File", 1, A_CHAR);
	if (r != ARCHIVE_OK)
		return (r);
	/* Application Identifier */
	r = set_file_identifier(bp, 575, 702, vdc, a, vdd,
	    &(iso9660->application_identifier),
	    "Application File", 1, A_CHAR);
	if (r != ARCHIVE_OK)
		return (r);
	/* Copyright File Identifier */
	r = set_file_identifier(bp, 703, 739, vdc, a, vdd,
	    &(iso9660->copyright_file_identifier),
	    "Copyright File", 0, D_CHAR);
	if (r != ARCHIVE_OK)
		return (r);
	/* Abstract File Identifier */
	r = set_file_identifier(bp, 740, 776, vdc, a, vdd,
	    &(iso9660->abstract_file_identifier),
	    "Abstract File", 0, D_CHAR);
	if (r != ARCHIVE_OK)
		return (r);
	/* Bibliongraphic File Identifier */
	r = set_file_identifier(bp, 777, 813, vdc, a, vdd,
	    &(iso9660->bibliographic_file_identifier),
	    "Bibliongraphic File", 0, D_CHAR);
	if (r != ARCHIVE_OK)
		return (r);
	/* Volume Creation Date and Time */
	set_date_time(bp+814, iso9660->birth_time);
	/* Volume Modification Date and Time */
	set_date_time(bp+831, iso9660->birth_time);
	/* Volume Expiration Date and Time(obsolete) */
	set_date_time_null(bp+848);
	/* Volume Effective Date and Time */
	set_date_time(bp+865, iso9660->birth_time);
	/* File Structure Version */
	bp[882] = fst_ver;
	/* Reserved */
	bp[883] = 0;
	/* Application Use */
	memset(bp + 884, 0x20, 1395 - 884 + 1);
	/* Reserved */
	set_unused_field_bp(bp, 1396, LOGICAL_BLOCK_SIZE);

	return (wb_consume(a, LOGICAL_BLOCK_SIZE));
}