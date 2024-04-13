static int nl80211_pre_doit(struct genl_ops *ops, struct sk_buff *skb,
			    struct genl_info *info)
{
	struct cfg80211_registered_device *rdev;
	struct wireless_dev *wdev;
	struct net_device *dev;
	bool rtnl = ops->internal_flags & NL80211_FLAG_NEED_RTNL;

	if (rtnl)
		rtnl_lock();

	if (ops->internal_flags & NL80211_FLAG_NEED_WIPHY) {
		rdev = cfg80211_get_dev_from_info(genl_info_net(info), info);
		if (IS_ERR(rdev)) {
			if (rtnl)
				rtnl_unlock();
			return PTR_ERR(rdev);
		}
		info->user_ptr[0] = rdev;
	} else if (ops->internal_flags & NL80211_FLAG_NEED_NETDEV ||
		   ops->internal_flags & NL80211_FLAG_NEED_WDEV) {
		mutex_lock(&cfg80211_mutex);
		wdev = __cfg80211_wdev_from_attrs(genl_info_net(info),
						  info->attrs);
		if (IS_ERR(wdev)) {
			mutex_unlock(&cfg80211_mutex);
			if (rtnl)
				rtnl_unlock();
			return PTR_ERR(wdev);
		}

		dev = wdev->netdev;
		rdev = wiphy_to_dev(wdev->wiphy);

		if (ops->internal_flags & NL80211_FLAG_NEED_NETDEV) {
			if (!dev) {
				mutex_unlock(&cfg80211_mutex);
				if (rtnl)
					rtnl_unlock();
				return -EINVAL;
			}

			info->user_ptr[1] = dev;
		} else {
			info->user_ptr[1] = wdev;
		}

		if (dev) {
			if (ops->internal_flags & NL80211_FLAG_CHECK_NETDEV_UP &&
			    !netif_running(dev)) {
				mutex_unlock(&cfg80211_mutex);
				if (rtnl)
					rtnl_unlock();
				return -ENETDOWN;
			}

			dev_hold(dev);
		} else if (ops->internal_flags & NL80211_FLAG_CHECK_NETDEV_UP) {
			if (!wdev->p2p_started) {
				mutex_unlock(&cfg80211_mutex);
				if (rtnl)
					rtnl_unlock();
				return -ENETDOWN;
			}
		}

		cfg80211_lock_rdev(rdev);

		mutex_unlock(&cfg80211_mutex);

		info->user_ptr[0] = rdev;
	}

	return 0;
}