zisofs_finish_entry(struct archive_write *a)
{
	struct iso9660 *iso9660 = a->format_data;
	struct isofile *file = iso9660->cur_file;
	unsigned char buff[16];
	size_t s;
	int64_t tail;

	/* Direct temp file stream to zisofs temp file stream. */
	archive_entry_set_size(file->entry, iso9660->zisofs.total_size);

	/*
	 * Save a file pointer which points the end of current zisofs data.
	 */
	tail = wb_offset(a);

	/*
	 * Make a header.
	 *
	 * +-----------------+----------------+-----------------+
	 * | Header 16 bytes | Block Pointers | Compressed data |
	 * +-----------------+----------------+-----------------+
	 * 0                16               +X
	 * Block Pointers :
	 *   4 * (((Uncompressed file size + block_size -1) / block_size) + 1)
	 *
	 * Write zisofs header.
	 *    Magic number
	 * +----+----+----+----+----+----+----+----+
	 * | 37 | E4 | 53 | 96 | C9 | DB | D6 | 07 |
	 * +----+----+----+----+----+----+----+----+
	 * 0    1    2    3    4    5    6    7    8
	 *
	 * +------------------------+------------------+
	 * | Uncompressed file size | header_size >> 2 |
	 * +------------------------+------------------+
	 * 8                       12                 13
	 *
	 * +-----------------+----------------+
	 * | log2 block_size | Reserved(0000) |
	 * +-----------------+----------------+
	 * 13               14               16
	 */
	memcpy(buff, zisofs_magic, 8);
	set_num_731(buff+8, file->zisofs.uncompressed_size);
	buff[12] = file->zisofs.header_size;
	buff[13] = file->zisofs.log2_bs;
	buff[14] = buff[15] = 0;/* Reserved */

	/* Move to the right position to write the header. */
	wb_set_offset(a, file->content.offset_of_temp);

	/* Write the header. */
	if (wb_write_to_temp(a, buff, 16) != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/*
	 * Write zisofs Block Pointers.
	 */
	s = iso9660->zisofs.block_pointers_cnt *
	    sizeof(iso9660->zisofs.block_pointers[0]);
	if (wb_write_to_temp(a, iso9660->zisofs.block_pointers, s)
	    != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/* Set a file pointer back to the end of the temporary file. */
	wb_set_offset(a, tail);

	return (ARCHIVE_OK);
}