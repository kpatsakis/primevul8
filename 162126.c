void cfg80211_testmode_event(struct sk_buff *skb, gfp_t gfp)
{
	void *hdr = ((void **)skb->cb)[1];
	struct nlattr *data = ((void **)skb->cb)[2];

	nla_nest_end(skb, data);
	genlmsg_end(skb, hdr);
	genlmsg_multicast(skb, 0, nl80211_testmode_mcgrp.id, gfp);
}