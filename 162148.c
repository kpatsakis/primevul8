static int nl80211_crypto_settings(struct cfg80211_registered_device *rdev,
				   struct genl_info *info,
				   struct cfg80211_crypto_settings *settings,
				   int cipher_limit)
{
	memset(settings, 0, sizeof(*settings));

	settings->control_port = info->attrs[NL80211_ATTR_CONTROL_PORT];

	if (info->attrs[NL80211_ATTR_CONTROL_PORT_ETHERTYPE]) {
		u16 proto;
		proto = nla_get_u16(
			info->attrs[NL80211_ATTR_CONTROL_PORT_ETHERTYPE]);
		settings->control_port_ethertype = cpu_to_be16(proto);
		if (!(rdev->wiphy.flags & WIPHY_FLAG_CONTROL_PORT_PROTOCOL) &&
		    proto != ETH_P_PAE)
			return -EINVAL;
		if (info->attrs[NL80211_ATTR_CONTROL_PORT_NO_ENCRYPT])
			settings->control_port_no_encrypt = true;
	} else
		settings->control_port_ethertype = cpu_to_be16(ETH_P_PAE);

	if (info->attrs[NL80211_ATTR_CIPHER_SUITES_PAIRWISE]) {
		void *data;
		int len, i;

		data = nla_data(info->attrs[NL80211_ATTR_CIPHER_SUITES_PAIRWISE]);
		len = nla_len(info->attrs[NL80211_ATTR_CIPHER_SUITES_PAIRWISE]);
		settings->n_ciphers_pairwise = len / sizeof(u32);

		if (len % sizeof(u32))
			return -EINVAL;

		if (settings->n_ciphers_pairwise > cipher_limit)
			return -EINVAL;

		memcpy(settings->ciphers_pairwise, data, len);

		for (i = 0; i < settings->n_ciphers_pairwise; i++)
			if (!cfg80211_supported_cipher_suite(
					&rdev->wiphy,
					settings->ciphers_pairwise[i]))
				return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_CIPHER_SUITE_GROUP]) {
		settings->cipher_group =
			nla_get_u32(info->attrs[NL80211_ATTR_CIPHER_SUITE_GROUP]);
		if (!cfg80211_supported_cipher_suite(&rdev->wiphy,
						     settings->cipher_group))
			return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_WPA_VERSIONS]) {
		settings->wpa_versions =
			nla_get_u32(info->attrs[NL80211_ATTR_WPA_VERSIONS]);
		if (!nl80211_valid_wpa_versions(settings->wpa_versions))
			return -EINVAL;
	}

	if (info->attrs[NL80211_ATTR_AKM_SUITES]) {
		void *data;
		int len;

		data = nla_data(info->attrs[NL80211_ATTR_AKM_SUITES]);
		len = nla_len(info->attrs[NL80211_ATTR_AKM_SUITES]);
		settings->n_akm_suites = len / sizeof(u32);

		if (len % sizeof(u32))
			return -EINVAL;

		if (settings->n_akm_suites > NL80211_MAX_NR_AKM_SUITES)
			return -EINVAL;

		memcpy(settings->akm_suites, data, len);
	}

	return 0;
}