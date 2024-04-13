int ping_get_port(struct sock *sk, unsigned short ident)
{
	struct hlist_nulls_node *node;
	struct hlist_nulls_head *hlist;
	struct inet_sock *isk, *isk2;
	struct sock *sk2 = NULL;

	isk = inet_sk(sk);
	write_lock_bh(&ping_table.lock);
	if (ident == 0) {
		u32 i;
		u16 result = ping_port_rover + 1;

		for (i = 0; i < (1L << 16); i++, result++) {
			if (!result)
				result++; /* avoid zero */
			hlist = ping_hashslot(&ping_table, sock_net(sk),
					    result);
			ping_portaddr_for_each_entry(sk2, node, hlist) {
				isk2 = inet_sk(sk2);

				if (isk2->inet_num == result)
					goto next_port;
			}

			/* found */
			ping_port_rover = ident = result;
			break;
next_port:
			;
		}
		if (i >= (1L << 16))
			goto fail;
	} else {
		hlist = ping_hashslot(&ping_table, sock_net(sk), ident);
		ping_portaddr_for_each_entry(sk2, node, hlist) {
			isk2 = inet_sk(sk2);

			/* BUG? Why is this reuse and not reuseaddr? ping.c
			 * doesn't turn off SO_REUSEADDR, and it doesn't expect
			 * that other ping processes can steal its packets.
			 */
			if ((isk2->inet_num == ident) &&
			    (sk2 != sk) &&
			    (!sk2->sk_reuse || !sk->sk_reuse))
				goto fail;
		}
	}

	pr_debug("found port/ident = %d\n", ident);
	isk->inet_num = ident;
	if (sk_unhashed(sk)) {
		pr_debug("was not hashed\n");
		sock_hold(sk);
		hlist_nulls_add_head(&sk->sk_nulls_node, hlist);
		sock_prot_inuse_add(sock_net(sk), sk->sk_prot, 1);
	}
	write_unlock_bh(&ping_table.lock);
	return 0;

fail:
	write_unlock_bh(&ping_table.lock);
	return 1;
}