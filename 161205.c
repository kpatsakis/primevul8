isoent_find_out_boot_file(struct archive_write *a, struct isoent *rootent)
{
	struct iso9660 *iso9660 = a->format_data;

	/* Find a isoent of the boot file. */
	iso9660->el_torito.boot = isoent_find_entry(rootent,
	    iso9660->el_torito.boot_filename.s);
	if (iso9660->el_torito.boot == NULL) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Can't find the boot image file ``%s''",
		    iso9660->el_torito.boot_filename.s);
		return (ARCHIVE_FATAL);
	}
	iso9660->el_torito.boot->file->boot = BOOT_IMAGE;
	return (ARCHIVE_OK);
}