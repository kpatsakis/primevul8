int cfg80211_testmode_reply(struct sk_buff *skb)
{
	struct cfg80211_registered_device *rdev = ((void **)skb->cb)[0];
	void *hdr = ((void **)skb->cb)[1];
	struct nlattr *data = ((void **)skb->cb)[2];

	if (WARN_ON(!rdev->testmode_info)) {
		kfree_skb(skb);
		return -EINVAL;
	}

	nla_nest_end(skb, data);
	genlmsg_end(skb, hdr);
	return genlmsg_reply(skb, rdev->testmode_info);
}