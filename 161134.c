zisofs_init_zstream(struct archive_write *a)
{
	struct iso9660 *iso9660 = a->format_data;
	int r;

	iso9660->zisofs.stream.next_in = NULL;
	iso9660->zisofs.stream.avail_in = 0;
	iso9660->zisofs.stream.total_in = 0;
	iso9660->zisofs.stream.total_out = 0;
	if (iso9660->zisofs.stream_valid)
		r = deflateReset(&(iso9660->zisofs.stream));
	else {
		r = deflateInit(&(iso9660->zisofs.stream),
		    iso9660->zisofs.compression_level);
		iso9660->zisofs.stream_valid = 1;
	}
	switch (r) {
	case Z_OK:
		break;
	default:
	case Z_STREAM_ERROR:
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Internal error initializing "
		    "compression library: invalid setup parameter");
		return (ARCHIVE_FATAL);
	case Z_MEM_ERROR:
		archive_set_error(&a->archive, ENOMEM,
		    "Internal error initializing "
		    "compression library");
		return (ARCHIVE_FATAL);
	case Z_VERSION_ERROR:
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Internal error initializing "
		    "compression library: invalid library version");
		return (ARCHIVE_FATAL);
	}
	return (ARCHIVE_OK);
}