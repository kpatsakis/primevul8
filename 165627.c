zip_read_data_zipx_xz(struct archive_read *a, const void **buff,
	size_t *size, int64_t *offset)
{
	struct zip* zip = (struct zip *)(a->format->data);
	int ret;
	lzma_ret lz_ret;
	const void* compressed_buf;
	ssize_t bytes_avail, in_bytes, to_consume = 0;

	(void) offset; /* UNUSED */

	/* Initialize decompressor if not yet initialized. */
	if (!zip->decompress_init) {
		ret = zipx_xz_init(a, zip);
		if (ret != ARCHIVE_OK)
			return (ret);
	}

	compressed_buf = __archive_read_ahead(a, 1, &bytes_avail);
	if (bytes_avail < 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated xz file body");
		return (ARCHIVE_FATAL);
	}

	in_bytes = zipmin(zip->entry_bytes_remaining, bytes_avail);
	zip->zipx_lzma_stream.next_in = compressed_buf;
	zip->zipx_lzma_stream.avail_in = in_bytes;
	zip->zipx_lzma_stream.total_in = 0;
	zip->zipx_lzma_stream.next_out = zip->uncompressed_buffer;
	zip->zipx_lzma_stream.avail_out = zip->uncompressed_buffer_size;
	zip->zipx_lzma_stream.total_out = 0;

	/* Perform the decompression. */
	lz_ret = lzma_code(&zip->zipx_lzma_stream, LZMA_RUN);
	switch(lz_ret) {
		case LZMA_DATA_ERROR:
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "xz data error (error %d)", (int) lz_ret);
			return (ARCHIVE_FATAL);

		case LZMA_NO_CHECK:
		case LZMA_OK:
			break;

		default:
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "xz unknown error %d", (int) lz_ret);
			return (ARCHIVE_FATAL);

		case LZMA_STREAM_END:
			lzma_end(&zip->zipx_lzma_stream);
			zip->zipx_lzma_valid = 0;

			if((int64_t) zip->zipx_lzma_stream.total_in !=
			    zip->entry_bytes_remaining)
			{
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_MISC,
				    "xz premature end of stream");
				return (ARCHIVE_FATAL);
			}

			zip->end_of_entry = 1;
			break;
	}

	to_consume = zip->zipx_lzma_stream.total_in;

	__archive_read_consume(a, to_consume);
	zip->entry_bytes_remaining -= to_consume;
	zip->entry_compressed_bytes_read += to_consume;
	zip->entry_uncompressed_bytes_read += zip->zipx_lzma_stream.total_out;

	*size = zip->zipx_lzma_stream.total_out;
	*buff = zip->uncompressed_buffer;

	ret = consume_optional_marker(a, zip);
	if (ret != ARCHIVE_OK)
		return (ret);

	return (ARCHIVE_OK);
}