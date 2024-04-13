zisofs_rewind_boot_file(struct archive_write *a)
{
	struct iso9660 *iso9660 = a->format_data;

	if (iso9660->el_torito.boot->file->zisofs.header_size != 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "We cannot extract the zisofs imaged boot file;"
		    " this may not boot in being zisofs imaged");
		return (ARCHIVE_FAILED);
	}
	return (ARCHIVE_OK);
}