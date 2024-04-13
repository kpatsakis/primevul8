zip_read_data_zipx_lzma_alone(struct archive_read *a, const void **buff,
    size_t *size, int64_t *offset)
{
	struct zip* zip = (struct zip *)(a->format->data);
	int ret;
	lzma_ret lz_ret;
	const void* compressed_buf;
	ssize_t bytes_avail, in_bytes, to_consume;

	(void) offset; /* UNUSED */

	/* Initialize decompressor if not yet initialized. */
	if (!zip->decompress_init) {
		ret = zipx_lzma_alone_init(a, zip);
		if (ret != ARCHIVE_OK)
			return (ret);
	}

	/* Fetch more compressed data. The same note as in deflate handler
	 * applies here as well:
	 *
	 * Note: '1' here is a performance optimization. Recall that the
	 * decompression layer returns a count of available bytes; asking for
	 * more than that forces the decompressor to combine reads by copying
	 * data.
	 */
	compressed_buf = __archive_read_ahead(a, 1, &bytes_avail);
	if (bytes_avail < 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated lzma file body");
		return (ARCHIVE_FATAL);
	}

	/* Set decompressor parameters. */
	in_bytes = zipmin(zip->entry_bytes_remaining, bytes_avail);

	zip->zipx_lzma_stream.next_in = compressed_buf;
	zip->zipx_lzma_stream.avail_in = in_bytes;
	zip->zipx_lzma_stream.total_in = 0;
	zip->zipx_lzma_stream.next_out = zip->uncompressed_buffer;
	zip->zipx_lzma_stream.avail_out =
		/* These lzma_alone streams lack end of stream marker, so let's
		 * make sure the unpacker won't try to unpack more than it's
		 * supposed to. */
		zipmin((int64_t) zip->uncompressed_buffer_size,
		    zip->entry->uncompressed_size -
		    zip->entry_uncompressed_bytes_read);
	zip->zipx_lzma_stream.total_out = 0;

	/* Perform the decompression. */
	lz_ret = lzma_code(&zip->zipx_lzma_stream, LZMA_RUN);
	switch(lz_ret) {
		case LZMA_DATA_ERROR:
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "lzma data error (error %d)", (int) lz_ret);
			return (ARCHIVE_FATAL);

		/* This case is optional in lzma alone format. It can happen,
		 * but most of the files don't have it. (GitHub #1257) */
		case LZMA_STREAM_END:
			lzma_end(&zip->zipx_lzma_stream);
			zip->zipx_lzma_valid = 0;
			if((int64_t) zip->zipx_lzma_stream.total_in !=
			    zip->entry_bytes_remaining)
			{
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_MISC,
				    "lzma alone premature end of stream");
				return (ARCHIVE_FATAL);
			}

			zip->end_of_entry = 1;
			break;

		case LZMA_OK:
			break;

		default:
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "lzma unknown error %d", (int) lz_ret);
			return (ARCHIVE_FATAL);
	}

	to_consume = zip->zipx_lzma_stream.total_in;

	/* Update pointers. */
	__archive_read_consume(a, to_consume);
	zip->entry_bytes_remaining -= to_consume;
	zip->entry_compressed_bytes_read += to_consume;
	zip->entry_uncompressed_bytes_read += zip->zipx_lzma_stream.total_out;

	if(zip->entry_bytes_remaining == 0) {
		zip->end_of_entry = 1;
	}

	/* Return values. */
	*size = zip->zipx_lzma_stream.total_out;
	*buff = zip->uncompressed_buffer;

	/* Behave the same way as during deflate decompression. */
	ret = consume_optional_marker(a, zip);
	if (ret != ARCHIVE_OK)
		return (ret);

	/* Free lzma decoder handle because we'll no longer need it. */
	if(zip->end_of_entry) {
		lzma_end(&zip->zipx_lzma_stream);
		zip->zipx_lzma_valid = 0;
	}

	/* If we're here, then we're good! */
	return (ARCHIVE_OK);
}