static int rtnl_link_fill(struct sk_buff *skb, const struct net_device *dev)
{
	struct nlattr *linkinfo;
	int err = -EMSGSIZE;

	linkinfo = nla_nest_start(skb, IFLA_LINKINFO);
	if (linkinfo == NULL)
		goto out;

	err = rtnl_link_info_fill(skb, dev);
	if (err < 0)
		goto err_cancel_link;

	err = rtnl_link_slave_info_fill(skb, dev);
	if (err < 0)
		goto err_cancel_link;

	nla_nest_end(skb, linkinfo);
	return 0;

err_cancel_link:
	nla_nest_cancel(skb, linkinfo);
out:
	return err;
}