static int nl80211_get_ifidx(struct netlink_callback *cb)
{
	int res;

	res = nlmsg_parse(cb->nlh, GENL_HDRLEN + nl80211_fam.hdrsize,
			  nl80211_fam.attrbuf, nl80211_fam.maxattr,
			  nl80211_policy);
	if (res)
		return res;

	if (!nl80211_fam.attrbuf[NL80211_ATTR_IFINDEX])
		return -EINVAL;

	res = nla_get_u32(nl80211_fam.attrbuf[NL80211_ATTR_IFINDEX]);
	if (!res)
		return -EINVAL;
	return res;
}