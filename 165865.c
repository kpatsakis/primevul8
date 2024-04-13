static struct sock *ping_get_idx(struct seq_file *seq, loff_t pos)
{
	struct sock *sk = ping_get_first(seq, 0);

	if (sk)
		while (pos && (sk = ping_get_next(seq, sk)) != NULL)
			--pos;
	return pos ? NULL : sk;
}