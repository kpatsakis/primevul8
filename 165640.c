zip_read_data_zipx_ppmd(struct archive_read *a, const void **buff,
    size_t *size, int64_t *offset)
{
	struct zip* zip = (struct zip *)(a->format->data);
	int ret;
	size_t consumed_bytes = 0;
	ssize_t bytes_avail = 0;

	(void) offset; /* UNUSED */

	/* If we're here for the first time, initialize Ppmd8 decompression
	 * context first. */
	if(!zip->decompress_init) {
		ret = zipx_ppmd8_init(a, zip);
		if(ret != ARCHIVE_OK)
			return ret;
	}

	/* Fetch for more data. We're reading 1 byte here, but libarchive
	 * should prefetch more bytes. */
	(void) __archive_read_ahead(a, 1, &bytes_avail);
	if(bytes_avail < 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated PPMd8 file body");
		return (ARCHIVE_FATAL);
	}

	/* This counter will be updated inside ppmd_read(), which at one
	 * point will be called by Ppmd8_DecodeSymbol. */
	zip->zipx_ppmd_read_compressed = 0;

	/* Decompression loop. */
	do {
		int sym = __archive_ppmd8_functions.Ppmd8_DecodeSymbol(
		    &zip->ppmd8);
		if(sym < 0) {
			zip->end_of_entry = 1;
			break;
		}

		/* This field is set by ppmd_read() when there was no more data
		 * to be read. */
		if(zip->ppmd8_stream_failed) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Truncated PPMd8 file body");
			return (ARCHIVE_FATAL);
		}

		zip->uncompressed_buffer[consumed_bytes] = (uint8_t) sym;
		++consumed_bytes;
	} while(consumed_bytes < zip->uncompressed_buffer_size);

	/* Update pointers for libarchive. */
	*buff = zip->uncompressed_buffer;
	*size = consumed_bytes;

	/* Update pointers so we can continue decompression in another call. */
	zip->entry_bytes_remaining -= zip->zipx_ppmd_read_compressed;
	zip->entry_compressed_bytes_read += zip->zipx_ppmd_read_compressed;
	zip->entry_uncompressed_bytes_read += consumed_bytes;

	/* If we're at the end of stream, deinitialize Ppmd8 context. */
	if(zip->end_of_entry) {
		__archive_ppmd8_functions.Ppmd8_Free(&zip->ppmd8);
		zip->ppmd8_valid = 0;
	}

	/* Seek for optional marker, same way as in each zip entry. */
	ret = consume_optional_marker(a, zip);
	if (ret != ARCHIVE_OK)
		return ret;

	return ARCHIVE_OK;
}