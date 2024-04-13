void nl80211_send_reg_change_event(struct regulatory_request *request)
{
	struct sk_buff *msg;
	void *hdr;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return;

	hdr = nl80211hdr_put(msg, 0, 0, 0, NL80211_CMD_REG_CHANGE);
	if (!hdr) {
		nlmsg_free(msg);
		return;
	}

	/* Userspace can always count this one always being set */
	if (nla_put_u8(msg, NL80211_ATTR_REG_INITIATOR, request->initiator))
		goto nla_put_failure;

	if (request->alpha2[0] == '0' && request->alpha2[1] == '0') {
		if (nla_put_u8(msg, NL80211_ATTR_REG_TYPE,
			       NL80211_REGDOM_TYPE_WORLD))
			goto nla_put_failure;
	} else if (request->alpha2[0] == '9' && request->alpha2[1] == '9') {
		if (nla_put_u8(msg, NL80211_ATTR_REG_TYPE,
			       NL80211_REGDOM_TYPE_CUSTOM_WORLD))
			goto nla_put_failure;
	} else if ((request->alpha2[0] == '9' && request->alpha2[1] == '8') ||
		   request->intersect) {
		if (nla_put_u8(msg, NL80211_ATTR_REG_TYPE,
			       NL80211_REGDOM_TYPE_INTERSECTION))
			goto nla_put_failure;
	} else {
		if (nla_put_u8(msg, NL80211_ATTR_REG_TYPE,
			       NL80211_REGDOM_TYPE_COUNTRY) ||
		    nla_put_string(msg, NL80211_ATTR_REG_ALPHA2,
				   request->alpha2))
			goto nla_put_failure;
	}

	if (request->wiphy_idx != WIPHY_IDX_INVALID &&
	    nla_put_u32(msg, NL80211_ATTR_WIPHY, request->wiphy_idx))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);

	rcu_read_lock();
	genlmsg_multicast_allns(msg, 0, nl80211_regulatory_mcgrp.id,
				GFP_ATOMIC);
	rcu_read_unlock();

	return;

nla_put_failure:
	genlmsg_cancel(msg, hdr);
	nlmsg_free(msg);
}