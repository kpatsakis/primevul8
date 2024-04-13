static int rtnl_link_info_fill(struct sk_buff *skb,
			       const struct net_device *dev)
{
	const struct rtnl_link_ops *ops = dev->rtnl_link_ops;
	struct nlattr *data;
	int err;

	if (!ops)
		return 0;
	if (nla_put_string(skb, IFLA_INFO_KIND, ops->kind) < 0)
		return -EMSGSIZE;
	if (ops->fill_xstats) {
		err = ops->fill_xstats(skb, dev);
		if (err < 0)
			return err;
	}
	if (ops->fill_info) {
		data = nla_nest_start(skb, IFLA_INFO_DATA);
		if (data == NULL)
			return -EMSGSIZE;
		err = ops->fill_info(skb, dev);
		if (err < 0)
			goto err_cancel_data;
		nla_nest_end(skb, data);
	}
	return 0;

err_cancel_data:
	nla_nest_cancel(skb, data);
	return err;
}