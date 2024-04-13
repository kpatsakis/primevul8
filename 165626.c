zipx_bzip2_init(struct archive_read *a, struct zip *zip)
{
	int r;

	/* Deallocate already existing BZ2 decompression context if it
	 * exists. */
	if(zip->bzstream_valid) {
		BZ2_bzDecompressEnd(&zip->bzstream);
		zip->bzstream_valid = 0;
	}

	/* Allocate a new BZ2 decompression context. */
	memset(&zip->bzstream, 0, sizeof(bz_stream));
	r = BZ2_bzDecompressInit(&zip->bzstream, 0, 1);
	if(r != BZ_OK) {
		archive_set_error(&(a->archive), ARCHIVE_ERRNO_MISC,
		    "bzip2 initialization failed(%d)",
		    r);

		return ARCHIVE_FAILED;
	}

	/* Mark the bzstream field to be released in cleanup phase. */
	zip->bzstream_valid = 1;

	/* (Re)allocate the buffer that will contain decompressed bytes. */
	free(zip->uncompressed_buffer);

	zip->uncompressed_buffer_size = 256 * 1024;
	zip->uncompressed_buffer =
	    (uint8_t*) malloc(zip->uncompressed_buffer_size);
	if (zip->uncompressed_buffer == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "No memory for bzip2 decompression");
		    return ARCHIVE_FATAL;
	}

	/* Initialization done. */
	zip->decompress_init = 1;
	return ARCHIVE_OK;
}