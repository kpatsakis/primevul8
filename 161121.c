extra_open_record(unsigned char *bp, int dr_len, struct isoent *isoent,
    struct ctl_extr_rec *ctl)
{
	ctl->bp = bp;
	if (bp != NULL)
		bp += dr_len;
	ctl->use_extr = 0;
	ctl->isoent = isoent;
	ctl->ce_ptr = NULL;
	ctl->cur_len = ctl->dr_len = dr_len;
	ctl->limit = DR_LIMIT;

	return (bp);
}