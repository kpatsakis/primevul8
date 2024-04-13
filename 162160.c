__cfg80211_testmode_alloc_skb(struct cfg80211_registered_device *rdev,
			      int approxlen, u32 portid, u32 seq, gfp_t gfp)
{
	struct sk_buff *skb;
	void *hdr;
	struct nlattr *data;

	skb = nlmsg_new(approxlen + 100, gfp);
	if (!skb)
		return NULL;

	hdr = nl80211hdr_put(skb, portid, seq, 0, NL80211_CMD_TESTMODE);
	if (!hdr) {
		kfree_skb(skb);
		return NULL;
	}

	if (nla_put_u32(skb, NL80211_ATTR_WIPHY, rdev->wiphy_idx))
		goto nla_put_failure;
	data = nla_nest_start(skb, NL80211_ATTR_TESTDATA);

	((void **)skb->cb)[0] = rdev;
	((void **)skb->cb)[1] = hdr;
	((void **)skb->cb)[2] = data;

	return skb;

 nla_put_failure:
	kfree_skb(skb);
	return NULL;
}