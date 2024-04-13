static void nl80211_post_doit(struct genl_ops *ops, struct sk_buff *skb,
			      struct genl_info *info)
{
	if (info->user_ptr[0])
		cfg80211_unlock_rdev(info->user_ptr[0]);
	if (info->user_ptr[1]) {
		if (ops->internal_flags & NL80211_FLAG_NEED_WDEV) {
			struct wireless_dev *wdev = info->user_ptr[1];

			if (wdev->netdev)
				dev_put(wdev->netdev);
		} else {
			dev_put(info->user_ptr[1]);
		}
	}
	if (ops->internal_flags & NL80211_FLAG_NEED_RTNL)
		rtnl_unlock();
}