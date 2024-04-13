void *ping_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct sock *sk;

	if (v == SEQ_START_TOKEN)
		sk = ping_get_idx(seq, 0);
	else
		sk = ping_get_next(seq, v);

	++*pos;
	return sk;
}