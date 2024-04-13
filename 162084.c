static int nl80211_dump_interface(struct sk_buff *skb, struct netlink_callback *cb)
{
	int wp_idx = 0;
	int if_idx = 0;
	int wp_start = cb->args[0];
	int if_start = cb->args[1];
	struct cfg80211_registered_device *rdev;
	struct wireless_dev *wdev;

	mutex_lock(&cfg80211_mutex);
	list_for_each_entry(rdev, &cfg80211_rdev_list, list) {
		if (!net_eq(wiphy_net(&rdev->wiphy), sock_net(skb->sk)))
			continue;
		if (wp_idx < wp_start) {
			wp_idx++;
			continue;
		}
		if_idx = 0;

		mutex_lock(&rdev->devlist_mtx);
		list_for_each_entry(wdev, &rdev->wdev_list, list) {
			if (if_idx < if_start) {
				if_idx++;
				continue;
			}
			if (nl80211_send_iface(skb, NETLINK_CB(cb->skb).portid,
					       cb->nlh->nlmsg_seq, NLM_F_MULTI,
					       rdev, wdev) < 0) {
				mutex_unlock(&rdev->devlist_mtx);
				goto out;
			}
			if_idx++;
		}
		mutex_unlock(&rdev->devlist_mtx);

		wp_idx++;
	}
 out:
	mutex_unlock(&cfg80211_mutex);

	cb->args[0] = wp_idx;
	cb->args[1] = if_idx;

	return skb->len;
}