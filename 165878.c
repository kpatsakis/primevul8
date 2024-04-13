static void *ping_v4_seq_start(struct seq_file *seq, loff_t *pos)
{
	return ping_seq_start(seq, pos, AF_INET);
}