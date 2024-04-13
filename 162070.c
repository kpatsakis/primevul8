static int nl80211_dump_scan(struct sk_buff *skb,
			     struct netlink_callback *cb)
{
	struct cfg80211_registered_device *rdev;
	struct net_device *dev;
	struct cfg80211_internal_bss *scan;
	struct wireless_dev *wdev;
	int start = cb->args[1], idx = 0;
	int err;

	err = nl80211_prepare_netdev_dump(skb, cb, &rdev, &dev);
	if (err)
		return err;

	wdev = dev->ieee80211_ptr;

	wdev_lock(wdev);
	spin_lock_bh(&rdev->bss_lock);
	cfg80211_bss_expire(rdev);

	cb->seq = rdev->bss_generation;

	list_for_each_entry(scan, &rdev->bss_list, list) {
		if (++idx <= start)
			continue;
		if (nl80211_send_bss(skb, cb,
				cb->nlh->nlmsg_seq, NLM_F_MULTI,
				rdev, wdev, scan) < 0) {
			idx--;
			break;
		}
	}

	spin_unlock_bh(&rdev->bss_lock);
	wdev_unlock(wdev);

	cb->args[1] = idx;
	nl80211_finish_netdev_dump(rdev);

	return skb->len;
}