void list_fields(unsigned int act_id)
{
	int i, j;
	unsigned int msk;
	char *hl;
	char hline[HEADER_LINE_LEN] = "";

	printf("# hostname;interval;timestamp");

	for (i = 0; i < NR_ACT; i++) {

		if ((act_id != ALL_ACTIVITIES) && (act[i]->id != act_id))
			continue;

		if (IS_SELECTED(act[i]->options) && (act[i]->nr_ini > 0)) {
			if (!HAS_MULTIPLE_OUTPUTS(act[i]->options)) {
				printf(";%s", act[i]->hdr_line);
				if ((act[i]->nr_ini > 1) && DISPLAY_HORIZONTALLY(flags)) {
					printf("[...]");
				}
			}
			else {
				msk = 1;
				strncpy(hline, act[i]->hdr_line, HEADER_LINE_LEN - 1);
				hline[HEADER_LINE_LEN - 1] = '\0';
				for (hl = strtok(hline, "|"); hl; hl = strtok(NULL, "|"), msk <<= 1) {
					if ((hl != NULL) && ((act[i]->opt_flags & 0xff) & msk)) {
						if (strchr(hl, '&')) {
							j = strcspn(hl, "&");
							if ((act[i]->opt_flags & 0xff00) & (msk << 8)) {
								/* Display whole header line */
								*(hl + j) = ';';
								printf(";%s", hl);
							}
							else {
								/* Display only the first part of the header line */
								*(hl + j) = '\0';
								printf(";%s", hl);
							}
							*(hl + j) = '&';
						}
						else {
							printf(";%s", hl);
						}
						if ((act[i]->nr_ini > 1) && DISPLAY_HORIZONTALLY(flags)) {
							printf("[...]");
						}
					}
				}
			}
		}
	}
	printf("\n");
}