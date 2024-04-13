static int sco_sock_accept(struct socket *sock, struct socket *newsock,
			   int flags)
{
	DEFINE_WAIT_FUNC(wait, woken_wake_function);
	struct sock *sk = sock->sk, *ch;
	long timeo;
	int err = 0;

	lock_sock(sk);

	timeo = sock_rcvtimeo(sk, flags & O_NONBLOCK);

	BT_DBG("sk %p timeo %ld", sk, timeo);

	/* Wait for an incoming connection. (wake-one). */
	add_wait_queue_exclusive(sk_sleep(sk), &wait);
	while (1) {
		if (sk->sk_state != BT_LISTEN) {
			err = -EBADFD;
			break;
		}

		ch = bt_accept_dequeue(sk, newsock);
		if (ch)
			break;

		if (!timeo) {
			err = -EAGAIN;
			break;
		}

		if (signal_pending(current)) {
			err = sock_intr_errno(timeo);
			break;
		}

		release_sock(sk);

		timeo = wait_woken(&wait, TASK_INTERRUPTIBLE, timeo);
		lock_sock(sk);
	}
	remove_wait_queue(sk_sleep(sk), &wait);

	if (err)
		goto done;

	newsock->state = SS_CONNECTED;

	BT_DBG("new socket %p", ch);

done:
	release_sock(sk);
	return err;
}