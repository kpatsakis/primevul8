nl80211_parse_connkeys(struct cfg80211_registered_device *rdev,
		       struct nlattr *keys, bool *no_ht)
{
	struct key_parse parse;
	struct nlattr *key;
	struct cfg80211_cached_keys *result;
	int rem, err, def = 0;

	result = kzalloc(sizeof(*result), GFP_KERNEL);
	if (!result)
		return ERR_PTR(-ENOMEM);

	result->def = -1;
	result->defmgmt = -1;

	nla_for_each_nested(key, keys, rem) {
		memset(&parse, 0, sizeof(parse));
		parse.idx = -1;

		err = nl80211_parse_key_new(key, &parse);
		if (err)
			goto error;
		err = -EINVAL;
		if (!parse.p.key)
			goto error;
		if (parse.idx < 0 || parse.idx > 4)
			goto error;
		if (parse.def) {
			if (def)
				goto error;
			def = 1;
			result->def = parse.idx;
			if (!parse.def_uni || !parse.def_multi)
				goto error;
		} else if (parse.defmgmt)
			goto error;
		err = cfg80211_validate_key_settings(rdev, &parse.p,
						     parse.idx, false, NULL);
		if (err)
			goto error;
		result->params[parse.idx].cipher = parse.p.cipher;
		result->params[parse.idx].key_len = parse.p.key_len;
		result->params[parse.idx].key = result->data[parse.idx];
		memcpy(result->data[parse.idx], parse.p.key, parse.p.key_len);

		if (parse.p.cipher == WLAN_CIPHER_SUITE_WEP40 ||
		    parse.p.cipher == WLAN_CIPHER_SUITE_WEP104) {
			if (no_ht)
				*no_ht = true;
		}
	}

	return result;
 error:
	kfree(result);
	return ERR_PTR(err);
}