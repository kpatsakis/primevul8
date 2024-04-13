static int nl80211_parse_key(struct genl_info *info, struct key_parse *k)
{
	int err;

	memset(k, 0, sizeof(*k));
	k->idx = -1;
	k->type = -1;

	if (info->attrs[NL80211_ATTR_KEY])
		err = nl80211_parse_key_new(info->attrs[NL80211_ATTR_KEY], k);
	else
		err = nl80211_parse_key_old(info, k);

	if (err)
		return err;

	if (k->def && k->defmgmt)
		return -EINVAL;

	if (k->defmgmt) {
		if (k->def_uni || !k->def_multi)
			return -EINVAL;
	}

	if (k->idx != -1) {
		if (k->defmgmt) {
			if (k->idx < 4 || k->idx > 5)
				return -EINVAL;
		} else if (k->def) {
			if (k->idx < 0 || k->idx > 3)
				return -EINVAL;
		} else {
			if (k->idx < 0 || k->idx > 5)
				return -EINVAL;
		}
	}

	return 0;
}