zipx_zstd_init(struct archive_read *a, struct zip *zip)
{
	size_t r;

	/* Deallocate already existing Zstd decompression context if it
	 * exists. */
	if(zip->zstdstream_valid) {
		ZSTD_freeDStream(zip->zstdstream);
		zip->zstdstream_valid = 0;
	}

	/* Allocate a new Zstd decompression context. */
	zip->zstdstream = ZSTD_createDStream();

	r = ZSTD_initDStream(zip->zstdstream);
	if (ZSTD_isError(r)) {
		 archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			"Error initializing zstd decompressor: %s",
			ZSTD_getErrorName(r));

		return ARCHIVE_FAILED;
	}

	/* Mark the zstdstream field to be released in cleanup phase. */
	zip->zstdstream_valid = 1;

	/* (Re)allocate the buffer that will contain decompressed bytes. */
	free(zip->uncompressed_buffer);

	zip->uncompressed_buffer_size = ZSTD_DStreamOutSize();
	zip->uncompressed_buffer =
	    (uint8_t*) malloc(zip->uncompressed_buffer_size);
	if (zip->uncompressed_buffer == NULL) {
		archive_set_error(&a->archive, ENOMEM,
			"No memory for Zstd decompression");

		return ARCHIVE_FATAL;
	}

	/* Initialization done. */
	zip->decompress_init = 1;
	return ARCHIVE_OK;
}