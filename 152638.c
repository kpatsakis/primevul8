int get_svg_graph_nr(int ifd, char *file, struct file_magic *file_magic,
		     struct file_activity *file_actlst, struct tm *rectime,
		     struct tm *loctime, int *views_per_row, int *nr_act_dispd)
{
	int i, n, p, tot_g_nr = 0;

	*nr_act_dispd = 0;

	/* Count items in file */
	if (!count_file_items(ifd, file, file_magic, file_actlst, rectime, loctime))
		/* No record to display => No graph */
		return 0;

	for (i = 0; i < NR_ACT; i++) {
		if (!id_seq[i])
			continue;

		p = get_activity_position(act, id_seq[i], EXIT_IF_NOT_FOUND);
		if (!IS_SELECTED(act[p]->options) || !act[p]->g_nr)
			continue;

		(*nr_act_dispd)++;

		if (PACK_VIEWS(flags)) {
			/*
			 * One activity = one row with multiple views.
			 * Exception is A_MEMORY, for which one activity may be
			 * displayed in two rows if both memory *and* swap utilization
			 * have been selected.
			 */
			if ((act[p]->id == A_MEMORY) &&
			    (DISPLAY_MEMORY(act[p]->opt_flags) && DISPLAY_SWAP(act[p]->opt_flags))) {
				n = 2;
			}
			else {
				n = 1;
			}
		}
		else {
			/* One activity = multiple rows with only one view */
			n = act[p]->g_nr;
		}
		if (ONE_GRAPH_PER_ITEM(act[p]->options)) {
			 n = n * act[p]->item_list_sz;
		}
		if (act[p]->g_nr > *views_per_row) {
			*views_per_row = act[p]->g_nr;
		}

		tot_g_nr += n;
	}

	if (*views_per_row > MAX_VIEWS_ON_A_ROW) {
		*views_per_row = MAX_VIEWS_ON_A_ROW;
	}

	return tot_g_nr;
}