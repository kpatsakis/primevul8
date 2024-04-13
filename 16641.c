static int irda_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	int err = -EOPNOTSUPP;

	IRDA_DEBUG(2, "%s()\n", __func__);

	lock_sock(sk);

	if ((sk->sk_type != SOCK_STREAM) && (sk->sk_type != SOCK_SEQPACKET) &&
	    (sk->sk_type != SOCK_DGRAM))
		goto out;

	if (sk->sk_state != TCP_LISTEN) {
		sk->sk_max_ack_backlog = backlog;
		sk->sk_state           = TCP_LISTEN;

		err = 0;
	}
out:
	release_sock(sk);

	return err;
}