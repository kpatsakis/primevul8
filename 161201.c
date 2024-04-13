setup_boot_information(struct archive_write *a)
{
	struct iso9660 *iso9660 = a->format_data;
	struct isoent *np;
	int64_t size;
	uint32_t sum;
	unsigned char buff[4096];

	np = iso9660->el_torito.boot;
	lseek(iso9660->temp_fd,
	    np->file->content.offset_of_temp + 64, SEEK_SET);
	size = archive_entry_size(np->file->entry) - 64;
	if (size <= 0) {
		archive_set_error(&a->archive, errno,
		    "Boot file(%jd) is too small", (intmax_t)size + 64);
		return (ARCHIVE_FATAL);
	}
	sum = 0;
	while (size > 0) {
		size_t rsize;
		ssize_t i, rs;

		if (size > (int64_t)sizeof(buff))
			rsize = sizeof(buff);
		else
			rsize = (size_t)size;

		rs = read(iso9660->temp_fd, buff, rsize);
		if (rs <= 0) {
			archive_set_error(&a->archive, errno,
			    "Can't read temporary file(%jd)",
			    (intmax_t)rs);
			return (ARCHIVE_FATAL);
		}
		for (i = 0; i < rs; i += 4)
			sum += archive_le32dec(buff + i);
		size -= rs;
	}
	/* Set the location of Primary Volume Descriptor. */
	set_num_731(buff, SYSTEM_AREA_BLOCK);
	/* Set the location of the boot file. */
	set_num_731(buff+4, np->file->content.location);
	/* Set the size of the boot file. */
	size = fd_boot_image_size(iso9660->el_torito.media_type);
	if (size == 0)
		size = archive_entry_size(np->file->entry);
	set_num_731(buff+8, (uint32_t)size);
	/* Set the sum of the boot file. */
	set_num_731(buff+12, sum);
	/* Clear reserved bytes. */
	memset(buff+16, 0, 40);

	/* Overwrite the boot file. */
	lseek(iso9660->temp_fd,
	    np->file->content.offset_of_temp + 8, SEEK_SET);
	return (write_to_temp(a, buff, 56));
}