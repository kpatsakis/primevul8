write_VD_boot_record(struct archive_write *a)
{
	struct iso9660 *iso9660;
	unsigned char *bp;

	iso9660 = a->format_data;
	bp = wb_buffptr(a) -1;
	/* Volume Descriptor Type */
	set_VD_bp(bp, VDT_BOOT_RECORD, 1);
	/* Boot System Identifier */
	memcpy(bp+8, "EL TORITO SPECIFICATION", 23);
	set_unused_field_bp(bp, 8+23, 39);
	/* Unused */
	set_unused_field_bp(bp, 40, 71);
	/* Absolute pointer to first sector of Boot Catalog */
	set_num_731(bp+72,
	    iso9660->el_torito.catalog->file->content.location);
	/* Unused */
	set_unused_field_bp(bp, 76, LOGICAL_BLOCK_SIZE);

	return (wb_consume(a, LOGICAL_BLOCK_SIZE));
}