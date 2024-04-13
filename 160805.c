static int kvp_file_init(void)
{
	int  fd;
	FILE *filep;
	size_t records_read;
	char *fname;
	struct kvp_record *record;
	struct kvp_record *readp;
	int num_blocks;
	int i;
	int alloc_unit = sizeof(struct kvp_record) * ENTRIES_PER_BLOCK;

	if (access("/var/opt/hyperv", F_OK)) {
		if (mkdir("/var/opt/hyperv", S_IRUSR | S_IWUSR | S_IROTH)) {
			syslog(LOG_ERR, " Failed to create /var/opt/hyperv");
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < KVP_POOL_COUNT; i++) {
		fname = kvp_file_info[i].fname;
		records_read = 0;
		num_blocks = 1;
		sprintf(fname, "/var/opt/hyperv/.kvp_pool_%d", i);
		fd = open(fname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH);

		if (fd == -1)
			return 1;


		filep = fopen(fname, "r");
		if (!filep)
			return 1;

		record = malloc(alloc_unit * num_blocks);
		if (record == NULL) {
			fclose(filep);
			return 1;
		}
		for (;;) {
			readp = &record[records_read];
			records_read += fread(readp, sizeof(struct kvp_record),
					ENTRIES_PER_BLOCK,
					filep);

			if (ferror(filep)) {
				syslog(LOG_ERR, "Failed to read file, pool: %d",
				       i);
				exit(EXIT_FAILURE);
			}

			if (!feof(filep)) {
				/*
				 * We have more data to read.
				 */
				num_blocks++;
				record = realloc(record, alloc_unit *
						num_blocks);
				if (record == NULL) {
					fclose(filep);
					return 1;
				}
				continue;
			}
			break;
		}
		kvp_file_info[i].fd = fd;
		kvp_file_info[i].num_blocks = num_blocks;
		kvp_file_info[i].records = record;
		kvp_file_info[i].num_records = records_read;
		fclose(filep);

	}

	return 0;
}