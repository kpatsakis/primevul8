zisofs_init(struct archive_write *a,  struct isofile *file)
{
	struct iso9660 *iso9660 = a->format_data;
#ifdef HAVE_ZLIB_H
	uint64_t tsize;
	size_t _ceil, bpsize;
	int r;
#endif

	iso9660->zisofs.detect_magic = 0;
	iso9660->zisofs.making = 0;

	if (!iso9660->opt.rr || !iso9660->opt.zisofs)
		return (ARCHIVE_OK);

	if (archive_entry_size(file->entry) >= 24 &&
	    archive_entry_size(file->entry) < MULTI_EXTENT_SIZE) {
		/* Acceptable file size for zisofs. */
		iso9660->zisofs.detect_magic = 1;
		iso9660->zisofs.magic_cnt = 0;
	}
	if (!iso9660->zisofs.detect_magic)
		return (ARCHIVE_OK);

#ifdef HAVE_ZLIB_H
	/* The number of Logical Blocks which uncompressed data
	 * will use in iso-image file is the same as the number of
	 * Logical Blocks which zisofs(compressed) data will use
	 * in ISO-image file. It won't reduce iso-image file size. */
	if (archive_entry_size(file->entry) <= LOGICAL_BLOCK_SIZE)
		return (ARCHIVE_OK);

	/* Initialize compression library */
	r = zisofs_init_zstream(a);
	if (r != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/* Mark file->zisofs to create RRIP 'ZF' Use Entry. */
	file->zisofs.header_size = ZF_HEADER_SIZE >> 2;
	file->zisofs.log2_bs = ZF_LOG2_BS;
	file->zisofs.uncompressed_size =
		(uint32_t)archive_entry_size(file->entry);

	/* Calculate a size of Block Pointers of zisofs. */
	_ceil = (file->zisofs.uncompressed_size + ZF_BLOCK_SIZE -1)
		>> file->zisofs.log2_bs;
	iso9660->zisofs.block_pointers_cnt = (int)_ceil + 1;
	iso9660->zisofs.block_pointers_idx = 0;

	/* Ensure a buffer size used for Block Pointers */
	bpsize = iso9660->zisofs.block_pointers_cnt *
	    sizeof(iso9660->zisofs.block_pointers[0]);
	if (iso9660->zisofs.block_pointers_allocated < bpsize) {
		free(iso9660->zisofs.block_pointers);
		iso9660->zisofs.block_pointers = malloc(bpsize);
		if (iso9660->zisofs.block_pointers == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate data");
			return (ARCHIVE_FATAL);
		}
		iso9660->zisofs.block_pointers_allocated = bpsize;
	}

	/*
	 * Skip zisofs header and Block Pointers, which we will write
	 * after all compressed data of a file written to the temporary
	 * file.
	 */
	tsize = ZF_HEADER_SIZE + bpsize;
	if (write_null(a, (size_t)tsize) != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/*
	 * Initialize some variables to make zisofs.
	 */
	archive_le32enc(&(iso9660->zisofs.block_pointers[0]),
		(uint32_t)tsize);
	iso9660->zisofs.remaining = file->zisofs.uncompressed_size;
	iso9660->zisofs.making = 1;
	iso9660->zisofs.allzero = 1;
	iso9660->zisofs.block_offset = tsize;
	iso9660->zisofs.total_size = tsize;
	iso9660->cur_file->cur_content->size = tsize;
#endif

	return (ARCHIVE_OK);
}