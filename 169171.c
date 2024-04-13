int read_record_hdr(int ifd, void *buffer, struct record_header *record_hdr,
		    struct file_header *file_hdr, int arch_64, int endian_mismatch,
		    int oneof, size_t b_size)
{
	int rc;

	do {
		if ((rc = sa_fread(ifd, buffer, (size_t) file_hdr->rec_size, SOFT_SIZE, oneof)) != 0)
			/* End of sa data file */
			return rc;

		/* Remap record header structure to that expected by current version */
		if (remap_struct(rec_types_nr, file_hdr->rec_types_nr, buffer,
				 file_hdr->rec_size, RECORD_HEADER_SIZE, b_size) < 0)
			return 2;
		memcpy(record_hdr, buffer, RECORD_HEADER_SIZE);

		/* Normalize endianness */
		if (endian_mismatch) {
			swap_struct(rec_types_nr, record_hdr, arch_64);
		}

		/*
		 * Skip unknown extra structures if present.
		 * This will be done later for R_COMMENT and R_RESTART records, as extra structures
		 * are saved after the comment or the number of CPU.
		 */
		if ((record_hdr->record_type != R_COMMENT) && (record_hdr->record_type != R_RESTART) &&
		    record_hdr->extra_next && (skip_extra_struct(ifd, endian_mismatch, arch_64) < 0))
			return 2;
	}
	while (record_hdr->record_type == R_EXTRA);

	return 0;
}