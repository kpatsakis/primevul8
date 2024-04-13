static int nl80211_set_reg(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr *tb[NL80211_REG_RULE_ATTR_MAX + 1];
	struct nlattr *nl_reg_rule;
	char *alpha2 = NULL;
	int rem_reg_rules = 0, r = 0;
	u32 num_rules = 0, rule_idx = 0, size_of_regd;
	u8 dfs_region = 0;
	struct ieee80211_regdomain *rd = NULL;

	if (!info->attrs[NL80211_ATTR_REG_ALPHA2])
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_REG_RULES])
		return -EINVAL;

	alpha2 = nla_data(info->attrs[NL80211_ATTR_REG_ALPHA2]);

	if (info->attrs[NL80211_ATTR_DFS_REGION])
		dfs_region = nla_get_u8(info->attrs[NL80211_ATTR_DFS_REGION]);

	nla_for_each_nested(nl_reg_rule, info->attrs[NL80211_ATTR_REG_RULES],
			    rem_reg_rules) {
		num_rules++;
		if (num_rules > NL80211_MAX_SUPP_REG_RULES)
			return -EINVAL;
	}

	size_of_regd = sizeof(struct ieee80211_regdomain) +
		       num_rules * sizeof(struct ieee80211_reg_rule);

	rd = kzalloc(size_of_regd, GFP_KERNEL);
	if (!rd)
		return -ENOMEM;

	rd->n_reg_rules = num_rules;
	rd->alpha2[0] = alpha2[0];
	rd->alpha2[1] = alpha2[1];

	/*
	 * Disable DFS master mode if the DFS region was
	 * not supported or known on this kernel.
	 */
	if (reg_supported_dfs_region(dfs_region))
		rd->dfs_region = dfs_region;

	nla_for_each_nested(nl_reg_rule, info->attrs[NL80211_ATTR_REG_RULES],
			    rem_reg_rules) {
		nla_parse(tb, NL80211_REG_RULE_ATTR_MAX,
			  nla_data(nl_reg_rule), nla_len(nl_reg_rule),
			  reg_rule_policy);
		r = parse_reg_rule(tb, &rd->reg_rules[rule_idx]);
		if (r)
			goto bad_reg;

		rule_idx++;

		if (rule_idx > NL80211_MAX_SUPP_REG_RULES) {
			r = -EINVAL;
			goto bad_reg;
		}
	}

	mutex_lock(&cfg80211_mutex);

	r = set_regdom(rd);
	/* set_regdom took ownership */
	rd = NULL;
	mutex_unlock(&cfg80211_mutex);

 bad_reg:
	kfree(rd);
	return r;
}