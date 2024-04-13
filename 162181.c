static int nl80211_testmode_dump(struct sk_buff *skb,
				 struct netlink_callback *cb)
{
	struct cfg80211_registered_device *rdev;
	int err;
	long phy_idx;
	void *data = NULL;
	int data_len = 0;

	if (cb->args[0]) {
		/*
		 * 0 is a valid index, but not valid for args[0],
		 * so we need to offset by 1.
		 */
		phy_idx = cb->args[0] - 1;
	} else {
		err = nlmsg_parse(cb->nlh, GENL_HDRLEN + nl80211_fam.hdrsize,
				  nl80211_fam.attrbuf, nl80211_fam.maxattr,
				  nl80211_policy);
		if (err)
			return err;

		mutex_lock(&cfg80211_mutex);
		rdev = __cfg80211_rdev_from_attrs(sock_net(skb->sk),
						  nl80211_fam.attrbuf);
		if (IS_ERR(rdev)) {
			mutex_unlock(&cfg80211_mutex);
			return PTR_ERR(rdev);
		}
		phy_idx = rdev->wiphy_idx;
		rdev = NULL;
		mutex_unlock(&cfg80211_mutex);

		if (nl80211_fam.attrbuf[NL80211_ATTR_TESTDATA])
			cb->args[1] =
				(long)nl80211_fam.attrbuf[NL80211_ATTR_TESTDATA];
	}

	if (cb->args[1]) {
		data = nla_data((void *)cb->args[1]);
		data_len = nla_len((void *)cb->args[1]);
	}

	mutex_lock(&cfg80211_mutex);
	rdev = cfg80211_rdev_by_wiphy_idx(phy_idx);
	if (!rdev) {
		mutex_unlock(&cfg80211_mutex);
		return -ENOENT;
	}
	cfg80211_lock_rdev(rdev);
	mutex_unlock(&cfg80211_mutex);

	if (!rdev->ops->testmode_dump) {
		err = -EOPNOTSUPP;
		goto out_err;
	}

	while (1) {
		void *hdr = nl80211hdr_put(skb, NETLINK_CB(cb->skb).portid,
					   cb->nlh->nlmsg_seq, NLM_F_MULTI,
					   NL80211_CMD_TESTMODE);
		struct nlattr *tmdata;

		if (nla_put_u32(skb, NL80211_ATTR_WIPHY, phy_idx)) {
			genlmsg_cancel(skb, hdr);
			break;
		}

		tmdata = nla_nest_start(skb, NL80211_ATTR_TESTDATA);
		if (!tmdata) {
			genlmsg_cancel(skb, hdr);
			break;
		}
		err = rdev_testmode_dump(rdev, skb, cb, data, data_len);
		nla_nest_end(skb, tmdata);

		if (err == -ENOBUFS || err == -ENOENT) {
			genlmsg_cancel(skb, hdr);
			break;
		} else if (err) {
			genlmsg_cancel(skb, hdr);
			goto out_err;
		}

		genlmsg_end(skb, hdr);
	}

	err = skb->len;
	/* see above */
	cb->args[0] = phy_idx + 1;
 out_err:
	cfg80211_unlock_rdev(rdev);
	return err;
}