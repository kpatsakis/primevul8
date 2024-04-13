static int irda_sendmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct irda_sock *self;
	struct sk_buff *skb;
	int err = -EPIPE;

	IRDA_DEBUG(4, "%s(), len=%zd\n", __func__, len);

	/* Note : socket.c set MSG_EOR on SEQPACKET sockets */
	if (msg->msg_flags & ~(MSG_DONTWAIT | MSG_EOR | MSG_CMSG_COMPAT |
			       MSG_NOSIGNAL)) {
		err = -EINVAL;
		goto out;
	}

	lock_sock(sk);

	if (sk->sk_shutdown & SEND_SHUTDOWN)
		goto out_err;

	if (sk->sk_state != TCP_ESTABLISHED) {
		err = -ENOTCONN;
		goto out;
	}

	self = irda_sk(sk);

	/* Check if IrTTP is wants us to slow down */

	if (wait_event_interruptible(*(sk_sleep(sk)),
	    (self->tx_flow != FLOW_STOP  ||  sk->sk_state != TCP_ESTABLISHED))) {
		err = -ERESTARTSYS;
		goto out;
	}

	/* Check if we are still connected */
	if (sk->sk_state != TCP_ESTABLISHED) {
		err = -ENOTCONN;
		goto out;
	}

	/* Check that we don't send out too big frames */
	if (len > self->max_data_size) {
		IRDA_DEBUG(2, "%s(), Chopping frame from %zd to %d bytes!\n",
			   __func__, len, self->max_data_size);
		len = self->max_data_size;
	}

	skb = sock_alloc_send_skb(sk, len + self->max_header_size + 16,
				  msg->msg_flags & MSG_DONTWAIT, &err);
	if (!skb)
		goto out_err;

	skb_reserve(skb, self->max_header_size + 16);
	skb_reset_transport_header(skb);
	skb_put(skb, len);
	err = memcpy_fromiovec(skb_transport_header(skb), msg->msg_iov, len);
	if (err) {
		kfree_skb(skb);
		goto out_err;
	}

	/*
	 * Just send the message to TinyTP, and let it deal with possible
	 * errors. No need to duplicate all that here
	 */
	err = irttp_data_request(self->tsap, skb);
	if (err) {
		IRDA_DEBUG(0, "%s(), err=%d\n", __func__, err);
		goto out_err;
	}

	release_sock(sk);
	/* Tell client how much data we actually sent */
	return len;

out_err:
	err = sk_stream_error(sk, msg->msg_flags, err);
out:
	release_sock(sk);
	return err;

}