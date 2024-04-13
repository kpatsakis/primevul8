static void __tcp_ack_snd_check(struct sock *sk, int ofo_possible)
{
	struct tcp_sock *tp = tcp_sk(sk);

	    /* More than one full frame received... */
	if (((tp->rcv_nxt - tp->rcv_wup) > inet_csk(sk)->icsk_ack.rcv_mss &&
	     /* ... and right edge of window advances far enough.
	      * (tcp_recvmsg() will send ACK otherwise). Or...
	      */
	     __tcp_select_window(sk) >= tp->rcv_wnd) ||
	    /* We ACK each frame or... */
	    tcp_in_quickack_mode(sk) ||
	    /* We have out of order data. */
	    (ofo_possible && skb_peek(&tp->out_of_order_queue))) {
		/* Then ack it now */
		tcp_send_ack(sk);
	} else {
		/* Else, send delayed ack. */
		tcp_send_delayed_ack(sk);
	}
}