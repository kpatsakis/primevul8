void init_structures(void)
{
	int i;

	for (i = 0; i < 3; i++)
		memset(&record_hdr[i], 0, RECORD_HEADER_SIZE);
}