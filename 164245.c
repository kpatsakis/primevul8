static int atalk_getname(struct socket *sock, struct sockaddr *uaddr,
			 int peer)
{
	struct sockaddr_at sat;
	struct sock *sk = sock->sk;
	struct atalk_sock *at = at_sk(sk);
	int err;

	lock_sock(sk);
	err = -ENOBUFS;
	if (sock_flag(sk, SOCK_ZAPPED))
		if (atalk_autobind(sk) < 0)
			goto out;

	memset(&sat, 0, sizeof(sat));

	if (peer) {
		err = -ENOTCONN;
		if (sk->sk_state != TCP_ESTABLISHED)
			goto out;

		sat.sat_addr.s_net  = at->dest_net;
		sat.sat_addr.s_node = at->dest_node;
		sat.sat_port	    = at->dest_port;
	} else {
		sat.sat_addr.s_net  = at->src_net;
		sat.sat_addr.s_node = at->src_node;
		sat.sat_port	    = at->src_port;
	}

	sat.sat_family = AF_APPLETALK;
	memcpy(uaddr, &sat, sizeof(sat));
	err = sizeof(struct sockaddr_at);

out:
	release_sock(sk);
	return err;
}