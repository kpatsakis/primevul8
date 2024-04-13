extra_tell_used_size(struct ctl_extr_rec *ctl, int size)
{
	struct isoent *isoent;
	struct extr_rec *rec;

	if (ctl->use_extr) {
		isoent = ctl->isoent->parent;
		rec = isoent->extr_rec_list.current;
		if (rec != NULL)
			rec->offset += size;
	}
	ctl->cur_len += size;
}