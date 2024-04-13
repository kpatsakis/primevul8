__nr_t read_nr_value(int ifd, char *file, struct file_magic *file_magic,
		     int endian_mismatch, int arch_64, int non_zero)
{
	__nr_t value;

	sa_fread(ifd, &value, sizeof(__nr_t), HARD_SIZE, UEOF_STOP);

	/* Normalize endianness for file_activity structures */
	if (endian_mismatch) {
		nr_types_nr[2] = 1;
		swap_struct(nr_types_nr, &value, arch_64);
	}

	if ((non_zero && !value) || (value < 0)) {
#ifdef DEBUG
		fprintf(stderr, "%s: Value=%d\n",
			__FUNCTION__, value);
#endif
		/* Value number cannot be zero or negative */
		handle_invalid_sa_file(ifd, file_magic, file, 0);
	}

	return value;
}