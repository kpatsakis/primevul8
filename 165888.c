	__acquires(ping_table.lock)
{
	struct ping_iter_state *state = seq->private;
	state->bucket = 0;
	state->family = family;

	read_lock_bh(&ping_table.lock);

	return *pos ? ping_get_idx(seq, *pos-1) : SEQ_START_TOKEN;
}