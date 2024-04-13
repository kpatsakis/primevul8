static bool vhost_sock_zcopy(struct socket *sock)
{
	return unlikely(experimental_zcopytx) &&
		sock_flag(sock->sk, SOCK_ZEROCOPY);
}