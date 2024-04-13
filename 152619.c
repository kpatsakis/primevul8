void print_read_error(int error_code)
{
	switch (error_code) {

		case END_OF_DATA_UNEXPECTED:
			/* Happens when the data collector doesn't send enough data */
			fprintf(stderr, _("End of data collecting unexpected\n"));
			break;

		default:
			/* Strange data sent by sadc...! */
			fprintf(stderr, _("Inconsistent input data\n"));
			break;
	}
	exit(3);
}