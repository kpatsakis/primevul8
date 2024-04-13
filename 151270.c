int parse_timestamp(char *argv[], int *opt, struct tstamp *tse,
		    const char *def_timestamp)
{
	char timestamp[9];

	if (argv[++(*opt)]) {
		switch (strlen(argv[*opt])) {

			case 5:
				strncpy(timestamp, argv[(*opt)++], 5);
				timestamp[5] = '\0';
				strcat(timestamp, ":00");
				break;

			case 8:
				strncpy(timestamp, argv[(*opt)++], 8);
				break;

			default:
				strncpy(timestamp, def_timestamp, 8);
				break;
		}
	} else {
		strncpy(timestamp, def_timestamp, 8);
	}
	timestamp[8] = '\0';

	return decode_timestamp(timestamp, tse);
}