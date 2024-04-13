static int nl80211_add_scan_req(struct sk_buff *msg,
				struct cfg80211_registered_device *rdev)
{
	struct cfg80211_scan_request *req = rdev->scan_req;
	struct nlattr *nest;
	int i;

	ASSERT_RDEV_LOCK(rdev);

	if (WARN_ON(!req))
		return 0;

	nest = nla_nest_start(msg, NL80211_ATTR_SCAN_SSIDS);
	if (!nest)
		goto nla_put_failure;
	for (i = 0; i < req->n_ssids; i++) {
		if (nla_put(msg, i, req->ssids[i].ssid_len, req->ssids[i].ssid))
			goto nla_put_failure;
	}
	nla_nest_end(msg, nest);

	nest = nla_nest_start(msg, NL80211_ATTR_SCAN_FREQUENCIES);
	if (!nest)
		goto nla_put_failure;
	for (i = 0; i < req->n_channels; i++) {
		if (nla_put_u32(msg, i, req->channels[i]->center_freq))
			goto nla_put_failure;
	}
	nla_nest_end(msg, nest);

	if (req->ie &&
	    nla_put(msg, NL80211_ATTR_IE, req->ie_len, req->ie))
		goto nla_put_failure;

	if (req->flags)
		nla_put_u32(msg, NL80211_ATTR_SCAN_FLAGS, req->flags);

	return 0;
 nla_put_failure:
	return -ENOBUFS;
}