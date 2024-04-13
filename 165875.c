void ping_close(struct sock *sk, long timeout)
{
	pr_debug("ping_close(sk=%p,sk->num=%u)\n",
		 inet_sk(sk), inet_sk(sk)->inet_num);
	pr_debug("isk->refcnt = %d\n", sk->sk_refcnt.counter);

	sk_common_release(sk);
}