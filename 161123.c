extra_close_record(struct ctl_extr_rec *ctl, int ce_size)
{
	int padding = 0;

	if (ce_size > 0)
		extra_tell_used_size(ctl, ce_size);
	/* Padding. */
	if (ctl->cur_len & 0x01) {
		ctl->cur_len++;
		if (ctl->bp != NULL)
			ctl->bp[ctl->cur_len] = 0;
		padding = 1;
	}
	if (ctl->use_extr) {
		if (ctl->ce_ptr != NULL)
			set_SUSP_CE(ctl->ce_ptr, ctl->extr_loc,
			    ctl->extr_off, ctl->cur_len - padding);
	} else
		ctl->dr_len = ctl->cur_len;
}