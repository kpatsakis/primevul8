static int nl80211_get_reg(struct sk_buff *skb, struct genl_info *info)
{
	const struct ieee80211_regdomain *regdom;
	struct sk_buff *msg;
	void *hdr = NULL;
	struct nlattr *nl_reg_rules;
	unsigned int i;
	int err = -EINVAL;

	mutex_lock(&cfg80211_mutex);

	if (!cfg80211_regdomain)
		goto out;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg) {
		err = -ENOBUFS;
		goto out;
	}

	hdr = nl80211hdr_put(msg, info->snd_portid, info->snd_seq, 0,
			     NL80211_CMD_GET_REG);
	if (!hdr)
		goto put_failure;

	if (reg_last_request_cell_base() &&
	    nla_put_u32(msg, NL80211_ATTR_USER_REG_HINT_TYPE,
			NL80211_USER_REG_HINT_CELL_BASE))
		goto nla_put_failure;

	rcu_read_lock();
	regdom = rcu_dereference(cfg80211_regdomain);

	if (nla_put_string(msg, NL80211_ATTR_REG_ALPHA2, regdom->alpha2) ||
	    (regdom->dfs_region &&
	     nla_put_u8(msg, NL80211_ATTR_DFS_REGION, regdom->dfs_region)))
		goto nla_put_failure_rcu;

	nl_reg_rules = nla_nest_start(msg, NL80211_ATTR_REG_RULES);
	if (!nl_reg_rules)
		goto nla_put_failure_rcu;

	for (i = 0; i < regdom->n_reg_rules; i++) {
		struct nlattr *nl_reg_rule;
		const struct ieee80211_reg_rule *reg_rule;
		const struct ieee80211_freq_range *freq_range;
		const struct ieee80211_power_rule *power_rule;

		reg_rule = &regdom->reg_rules[i];
		freq_range = &reg_rule->freq_range;
		power_rule = &reg_rule->power_rule;

		nl_reg_rule = nla_nest_start(msg, i);
		if (!nl_reg_rule)
			goto nla_put_failure_rcu;

		if (nla_put_u32(msg, NL80211_ATTR_REG_RULE_FLAGS,
				reg_rule->flags) ||
		    nla_put_u32(msg, NL80211_ATTR_FREQ_RANGE_START,
				freq_range->start_freq_khz) ||
		    nla_put_u32(msg, NL80211_ATTR_FREQ_RANGE_END,
				freq_range->end_freq_khz) ||
		    nla_put_u32(msg, NL80211_ATTR_FREQ_RANGE_MAX_BW,
				freq_range->max_bandwidth_khz) ||
		    nla_put_u32(msg, NL80211_ATTR_POWER_RULE_MAX_ANT_GAIN,
				power_rule->max_antenna_gain) ||
		    nla_put_u32(msg, NL80211_ATTR_POWER_RULE_MAX_EIRP,
				power_rule->max_eirp))
			goto nla_put_failure_rcu;

		nla_nest_end(msg, nl_reg_rule);
	}
	rcu_read_unlock();

	nla_nest_end(msg, nl_reg_rules);

	genlmsg_end(msg, hdr);
	err = genlmsg_reply(msg, info);
	goto out;

nla_put_failure_rcu:
	rcu_read_unlock();
nla_put_failure:
	genlmsg_cancel(msg, hdr);
put_failure:
	nlmsg_free(msg);
	err = -EMSGSIZE;
out:
	mutex_unlock(&cfg80211_mutex);
	return err;
}