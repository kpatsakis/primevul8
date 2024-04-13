static int nl80211_put_iftypes(struct sk_buff *msg, u32 attr, u16 ifmodes)
{
	struct nlattr *nl_modes = nla_nest_start(msg, attr);
	int i;

	if (!nl_modes)
		goto nla_put_failure;

	i = 0;
	while (ifmodes) {
		if ((ifmodes & 1) && nla_put_flag(msg, i))
			goto nla_put_failure;
		ifmodes >>= 1;
		i++;
	}

	nla_nest_end(msg, nl_modes);
	return 0;

nla_put_failure:
	return -ENOBUFS;
}