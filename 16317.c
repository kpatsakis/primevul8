static bool io_net_retry(struct socket *sock, int flags)
{
	if (!(flags & MSG_WAITALL))
		return false;
	return sock->type == SOCK_STREAM || sock->type == SOCK_SEQPACKET;
}