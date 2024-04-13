int check_line_hdr(void)
{
	int i, rc = FALSE;

	/* Get number of options entered on the command line */
	if (get_activity_nr(act, AO_SELECTED, COUNT_OUTPUTS) > 1)
		return TRUE;

	for (i = 0; i < NR_ACT; i++) {
		if (IS_SELECTED(act[i]->options)) {
			/* Special processing for activities using a bitmap */
			if (act[i]->bitmap) {
				if (count_bits(act[i]->bitmap->b_array,
					       BITMAP_SIZE(act[i]->bitmap->b_size)) > 1) {
					rc = TRUE;
				}
			}
			else if (act[i]->nr_ini > 1) {
				rc = TRUE;
			}
			/* Stop now since we have only one selected activity */
			break;
		}
	}

	return rc;
}