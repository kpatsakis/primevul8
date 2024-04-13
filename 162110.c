static int nl80211_parse_key_old(struct genl_info *info, struct key_parse *k)
{
	if (info->attrs[NL80211_ATTR_KEY_DATA]) {
		k->p.key = nla_data(info->attrs[NL80211_ATTR_KEY_DATA]);
		k->p.key_len = nla_len(info->attrs[NL80211_ATTR_KEY_DATA]);
	}

	if (info->attrs[NL80211_ATTR_KEY_SEQ]) {
		k->p.seq = nla_data(info->attrs[NL80211_ATTR_KEY_SEQ]);
		k->p.seq_len = nla_len(info->attrs[NL80211_ATTR_KEY_SEQ]);
	}

	if (info->attrs[NL80211_ATTR_KEY_IDX])
		k->idx = nla_get_u8(info->attrs[NL80211_ATTR_KEY_IDX]);

	if (info->attrs[NL80211_ATTR_KEY_CIPHER])
		k->p.cipher = nla_get_u32(info->attrs[NL80211_ATTR_KEY_CIPHER]);

	k->def = !!info->attrs[NL80211_ATTR_KEY_DEFAULT];
	k->defmgmt = !!info->attrs[NL80211_ATTR_KEY_DEFAULT_MGMT];

	if (k->def) {
		k->def_uni = true;
		k->def_multi = true;
	}
	if (k->defmgmt)
		k->def_multi = true;

	if (info->attrs[NL80211_ATTR_KEY_TYPE]) {
		k->type = nla_get_u32(info->attrs[NL80211_ATTR_KEY_TYPE]);
		if (k->type < 0 || k->type >= NUM_NL80211_KEYTYPES)
			return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_KEY_DEFAULT_TYPES]) {
		struct nlattr *kdt[NUM_NL80211_KEY_DEFAULT_TYPES];
		int err = nla_parse_nested(
				kdt, NUM_NL80211_KEY_DEFAULT_TYPES - 1,
				info->attrs[NL80211_ATTR_KEY_DEFAULT_TYPES],
				nl80211_key_default_policy);
		if (err)
			return err;

		k->def_uni = kdt[NL80211_KEY_DEFAULT_TYPE_UNICAST];
		k->def_multi = kdt[NL80211_KEY_DEFAULT_TYPE_MULTICAST];
	}

	return 0;
}