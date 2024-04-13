static int nl80211_dump_survey(struct sk_buff *skb,
			struct netlink_callback *cb)
{
	struct survey_info survey;
	struct cfg80211_registered_device *dev;
	struct net_device *netdev;
	int survey_idx = cb->args[1];
	int res;

	res = nl80211_prepare_netdev_dump(skb, cb, &dev, &netdev);
	if (res)
		return res;

	if (!dev->ops->dump_survey) {
		res = -EOPNOTSUPP;
		goto out_err;
	}

	while (1) {
		struct ieee80211_channel *chan;

		res = rdev_dump_survey(dev, netdev, survey_idx, &survey);
		if (res == -ENOENT)
			break;
		if (res)
			goto out_err;

		/* Survey without a channel doesn't make sense */
		if (!survey.channel) {
			res = -EINVAL;
			goto out;
		}

		chan = ieee80211_get_channel(&dev->wiphy,
					     survey.channel->center_freq);
		if (!chan || chan->flags & IEEE80211_CHAN_DISABLED) {
			survey_idx++;
			continue;
		}

		if (nl80211_send_survey(skb,
				NETLINK_CB(cb->skb).portid,
				cb->nlh->nlmsg_seq, NLM_F_MULTI,
				netdev,
				&survey) < 0)
			goto out;
		survey_idx++;
	}

 out:
	cb->args[1] = survey_idx;
	res = skb->len;
 out_err:
	nl80211_finish_netdev_dump(dev);
	return res;
}