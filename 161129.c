zisofs_extract(struct archive_write *a, struct zisofs_extract *zisofs,
    const unsigned char *p, size_t bytes)
{
	size_t avail;
	int r;

	if (!zisofs->initialized) {
		ssize_t rs = zisofs_extract_init(a, zisofs, p, bytes);
		if (rs < 0)
			return (rs);
		if (!zisofs->initialized) {
			/* We need more data. */
			zisofs->pz_offset += (uint32_t)bytes;
			return (bytes);
		}
		avail = rs;
		p += bytes - avail;
	} else
		avail = bytes;

	/*
	 * Get block offsets from block pointers.
	 */
	if (zisofs->block_avail == 0) {
		uint32_t bst, bed;

		if (zisofs->block_off + 4 >= zisofs->block_pointers_size) {
			/* There isn't a pair of offsets. */
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Illegal zisofs block pointers");
			return (ARCHIVE_FATAL);
		}
		bst = archive_le32dec(
		    zisofs->block_pointers + zisofs->block_off);
		if (bst != zisofs->pz_offset + (bytes - avail)) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Illegal zisofs block pointers(cannot seek)");
			return (ARCHIVE_FATAL);
		}
		bed = archive_le32dec(
		    zisofs->block_pointers + zisofs->block_off + 4);
		if (bed < bst) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Illegal zisofs block pointers");
			return (ARCHIVE_FATAL);
		}
		zisofs->block_avail = bed - bst;
		zisofs->block_off += 4;

		/* Initialize compression library for new block. */
		if (zisofs->stream_valid)
			r = inflateReset(&zisofs->stream);
		else
			r = inflateInit(&zisofs->stream);
		if (r != Z_OK) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Can't initialize zisofs decompression.");
			return (ARCHIVE_FATAL);
		}
		zisofs->stream_valid = 1;
		zisofs->stream.total_in = 0;
		zisofs->stream.total_out = 0;
	}

	/*
	 * Make uncompressed data.
	 */
	if (zisofs->block_avail == 0) {
		/*
		 * It's basically 32K bytes NUL data.
		 */
		unsigned char *wb;
		size_t size, wsize;

		size = zisofs->uncompressed_buffer_size;
		while (size) {
			wb = wb_buffptr(a);
			if (size > wb_remaining(a))
				wsize = wb_remaining(a);
			else
				wsize = size;
			memset(wb, 0, wsize);
			r = wb_consume(a, wsize);
			if (r < 0)
				return (r);
			size -= wsize;
		}
	} else {
		zisofs->stream.next_in = (Bytef *)(uintptr_t)(const void *)p;
		if (avail > zisofs->block_avail)
			zisofs->stream.avail_in = zisofs->block_avail;
		else
			zisofs->stream.avail_in = (uInt)avail;
		zisofs->stream.next_out = wb_buffptr(a);
		zisofs->stream.avail_out = (uInt)wb_remaining(a);

		r = inflate(&zisofs->stream, 0);
		switch (r) {
		case Z_OK: /* Decompressor made some progress.*/
		case Z_STREAM_END: /* Found end of stream. */
			break;
		default:
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "zisofs decompression failed (%d)", r);
			return (ARCHIVE_FATAL);
		}
		avail -= zisofs->stream.next_in - p;
		zisofs->block_avail -= (uint32_t)(zisofs->stream.next_in - p);
		r = wb_consume(a, wb_remaining(a) - zisofs->stream.avail_out);
		if (r < 0)
			return (r);
	}
	zisofs->pz_offset += (uint32_t)bytes;
	return (bytes - avail);
}