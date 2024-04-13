void read_sadc_stat_bunch(int curr)
{
	int i, p;

	/* Read record header (type is always R_STATS since it is read from sadc) */
	if (sa_read(&record_hdr[curr], RECORD_HEADER_SIZE)) {
		/*
		 * SIGINT (sent by sadc) is likely to be received
		 * while we are stuck in sa_read().
		 * If this happens then no data have to be read.
		 */
		if (sigint_caught)
			return;

#ifdef DEBUG
		fprintf(stderr, "%s: Record header\n", __FUNCTION__);
#endif
		print_read_error(END_OF_DATA_UNEXPECTED);
	}

	for (i = 0; i < NR_ACT; i++) {

		if (!id_seq[i])
			continue;
		p = get_activity_position(act, id_seq[i], EXIT_IF_NOT_FOUND);

		if (HAS_COUNT_FUNCTION(act[p]->options)) {
			if (sa_read(&(act[p]->nr[curr]), sizeof(__nr_t))) {
#ifdef DEBUG
				fprintf(stderr, "%s: Nb of items\n", __FUNCTION__);
#endif
				print_read_error(END_OF_DATA_UNEXPECTED);
			}
			if ((act[p]->nr[curr] > act[p]->nr_max) || (act[p]->nr[curr] < 0)) {
#ifdef DEBUG
				fprintf(stderr, "%s: %s: nr=%d nr_max=%d\n",
					__FUNCTION__, act[p]->name, act[p]->nr[curr], act[p]->nr_max);
#endif
				print_read_error(INCONSISTENT_INPUT_DATA);
			}
			if (act[p]->nr[curr] > act[p]->nr_allocated) {
				reallocate_all_buffers(act[p], act[p]->nr[curr]);
			}


			/*
			 * For persistent activities, we must make sure that no statistics
                         * from a previous iteration remain, especially if the number
                         * of structures read is smaller than @nr_ini.
                         */
                        if (HAS_PERSISTENT_VALUES(act[p]->options)) {
                            memset(act[p]->buf[curr], 0,
                                   (size_t) act[p]->fsize * (size_t) act[p]->nr_ini * (size_t) act[p]->nr2);
                        }
                }
		if (sa_read(act[p]->buf[curr],
			    (size_t) act[p]->fsize * (size_t) act[p]->nr[curr] * (size_t) act[p]->nr2)) {
#ifdef DEBUG
			fprintf(stderr, "%s: Statistics\n", __FUNCTION__);
#endif
			print_read_error(END_OF_DATA_UNEXPECTED);
		}
	}
}