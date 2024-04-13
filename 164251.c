static void atalk_destroy_timer(struct timer_list *t)
{
	struct sock *sk = from_timer(sk, t, sk_timer);

	if (sk_has_allocations(sk)) {
		sk->sk_timer.expires = jiffies + SOCK_DESTROY_TIME;
		add_timer(&sk->sk_timer);
	} else
		sock_put(sk);
}