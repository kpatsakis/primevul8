static void get_key_callback(void *c, struct key_params *params)
{
	struct nlattr *key;
	struct get_key_cookie *cookie = c;

	if ((params->key &&
	     nla_put(cookie->msg, NL80211_ATTR_KEY_DATA,
		     params->key_len, params->key)) ||
	    (params->seq &&
	     nla_put(cookie->msg, NL80211_ATTR_KEY_SEQ,
		     params->seq_len, params->seq)) ||
	    (params->cipher &&
	     nla_put_u32(cookie->msg, NL80211_ATTR_KEY_CIPHER,
			 params->cipher)))
		goto nla_put_failure;

	key = nla_nest_start(cookie->msg, NL80211_ATTR_KEY);
	if (!key)
		goto nla_put_failure;

	if ((params->key &&
	     nla_put(cookie->msg, NL80211_KEY_DATA,
		     params->key_len, params->key)) ||
	    (params->seq &&
	     nla_put(cookie->msg, NL80211_KEY_SEQ,
		     params->seq_len, params->seq)) ||
	    (params->cipher &&
	     nla_put_u32(cookie->msg, NL80211_KEY_CIPHER,
			 params->cipher)))
		goto nla_put_failure;

	if (nla_put_u8(cookie->msg, NL80211_ATTR_KEY_IDX, cookie->idx))
		goto nla_put_failure;

	nla_nest_end(cookie->msg, key);

	return;
 nla_put_failure:
	cookie->error = 1;
}