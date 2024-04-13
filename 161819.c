static int selinux_netlink_send(struct sock *sk, struct sk_buff *skb)
{
	int err;

	err = cap_netlink_send(sk, skb);
	if (err)
		return err;

	return selinux_nlmsg_perm(sk, skb);
}