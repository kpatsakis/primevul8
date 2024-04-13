static int nl80211_put_iface_combinations(struct wiphy *wiphy,
					  struct sk_buff *msg)
{
	struct nlattr *nl_combis;
	int i, j;

	nl_combis = nla_nest_start(msg,
				NL80211_ATTR_INTERFACE_COMBINATIONS);
	if (!nl_combis)
		goto nla_put_failure;

	for (i = 0; i < wiphy->n_iface_combinations; i++) {
		const struct ieee80211_iface_combination *c;
		struct nlattr *nl_combi, *nl_limits;

		c = &wiphy->iface_combinations[i];

		nl_combi = nla_nest_start(msg, i + 1);
		if (!nl_combi)
			goto nla_put_failure;

		nl_limits = nla_nest_start(msg, NL80211_IFACE_COMB_LIMITS);
		if (!nl_limits)
			goto nla_put_failure;

		for (j = 0; j < c->n_limits; j++) {
			struct nlattr *nl_limit;

			nl_limit = nla_nest_start(msg, j + 1);
			if (!nl_limit)
				goto nla_put_failure;
			if (nla_put_u32(msg, NL80211_IFACE_LIMIT_MAX,
					c->limits[j].max))
				goto nla_put_failure;
			if (nl80211_put_iftypes(msg, NL80211_IFACE_LIMIT_TYPES,
						c->limits[j].types))
				goto nla_put_failure;
			nla_nest_end(msg, nl_limit);
		}

		nla_nest_end(msg, nl_limits);

		if (c->beacon_int_infra_match &&
		    nla_put_flag(msg, NL80211_IFACE_COMB_STA_AP_BI_MATCH))
			goto nla_put_failure;
		if (nla_put_u32(msg, NL80211_IFACE_COMB_NUM_CHANNELS,
				c->num_different_channels) ||
		    nla_put_u32(msg, NL80211_IFACE_COMB_MAXNUM,
				c->max_interfaces))
			goto nla_put_failure;

		nla_nest_end(msg, nl_combi);
	}

	nla_nest_end(msg, nl_combis);

	return 0;
nla_put_failure:
	return -ENOBUFS;
}