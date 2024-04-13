zipx_xz_init(struct archive_read *a, struct zip *zip)
{
	lzma_ret r;

	if(zip->zipx_lzma_valid) {
		lzma_end(&zip->zipx_lzma_stream);
		zip->zipx_lzma_valid = 0;
	}

	memset(&zip->zipx_lzma_stream, 0, sizeof(zip->zipx_lzma_stream));
	r = lzma_stream_decoder(&zip->zipx_lzma_stream, UINT64_MAX, 0);
	if (r != LZMA_OK) {
		archive_set_error(&(a->archive), ARCHIVE_ERRNO_MISC,
		    "xz initialization failed(%d)",
		    r);

		return (ARCHIVE_FAILED);
	}

	zip->zipx_lzma_valid = 1;

	free(zip->uncompressed_buffer);

	zip->uncompressed_buffer_size = 256 * 1024;
	zip->uncompressed_buffer =
	    (uint8_t*) malloc(zip->uncompressed_buffer_size);
	if (zip->uncompressed_buffer == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "No memory for xz decompression");
		    return (ARCHIVE_FATAL);
	}

	zip->decompress_init = 1;
	return (ARCHIVE_OK);
}