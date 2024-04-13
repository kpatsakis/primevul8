static int nl80211_dump_wiphy(struct sk_buff *skb, struct netlink_callback *cb)
{
	int idx = 0;
	int start = cb->args[0];
	struct cfg80211_registered_device *dev;

	mutex_lock(&cfg80211_mutex);
	list_for_each_entry(dev, &cfg80211_rdev_list, list) {
		if (!net_eq(wiphy_net(&dev->wiphy), sock_net(skb->sk)))
			continue;
		if (++idx <= start)
			continue;
		if (nl80211_send_wiphy(skb, NETLINK_CB(cb->skb).portid,
				       cb->nlh->nlmsg_seq, NLM_F_MULTI,
				       dev) < 0) {
			idx--;
			break;
		}
	}
	mutex_unlock(&cfg80211_mutex);

	cb->args[0] = idx;

	return skb->len;
}