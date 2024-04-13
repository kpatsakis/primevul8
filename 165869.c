static struct sock *ping_get_first(struct seq_file *seq, int start)
{
	struct sock *sk;
	struct ping_iter_state *state = seq->private;
	struct net *net = seq_file_net(seq);

	for (state->bucket = start; state->bucket < PING_HTABLE_SIZE;
	     ++state->bucket) {
		struct hlist_nulls_node *node;
		struct hlist_nulls_head *hslot;

		hslot = &ping_table.hash[state->bucket];

		if (hlist_nulls_empty(hslot))
			continue;

		sk_nulls_for_each(sk, node, hslot) {
			if (net_eq(sock_net(sk), net) &&
			    sk->sk_family == state->family)
				goto found;
		}
	}
	sk = NULL;
found:
	return sk;
}