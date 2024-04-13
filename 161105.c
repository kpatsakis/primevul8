extra_next_record(struct ctl_extr_rec *ctl, int length)
{
	int cur_len = ctl->cur_len;/* save cur_len */

	/* Close the current extra record or Directory Record. */
	extra_close_record(ctl, RR_CE_SIZE);

	/* Get a next extra record. */
	ctl->use_extr = 1;
	if (ctl->bp != NULL) {
		/* Storing data into an extra record. */
		unsigned char *p;

		/* Save the pointer where a CE extension will be
		 * stored to. */
		ctl->ce_ptr = &ctl->bp[cur_len+1];
		p = extra_get_record(ctl->isoent,
		    &ctl->limit, &ctl->extr_off, &ctl->extr_loc);
		ctl->bp = p - 1;/* the base of bp offset is 1. */
	} else
		/* Calculating the size of an extra record. */
		(void)extra_get_record(ctl->isoent,
		    &ctl->limit, NULL, NULL);
	ctl->cur_len = 0;
	/* Check if an extra record is almost full.
	 * If so, get a next one. */
	if (extra_space(ctl) < length)
		(void)extra_next_record(ctl, length);

	return (ctl->bp);
}