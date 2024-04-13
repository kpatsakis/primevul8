static int dgram_connect(struct sock *sk, struct sockaddr *uaddr,
			 int len)
{
	struct sockaddr_ieee802154 *addr = (struct sockaddr_ieee802154 *)uaddr;
	struct dgram_sock *ro = dgram_sk(sk);
	int err = 0;

	if (len < sizeof(*addr))
		return -EINVAL;

	if (addr->family != AF_IEEE802154)
		return -EINVAL;

	lock_sock(sk);

	if (!ro->bound) {
		err = -ENETUNREACH;
		goto out;
	}

	ieee802154_addr_from_sa(&ro->dst_addr, &addr->addr);
	ro->connected = 1;

out:
	release_sock(sk);
	return err;
}