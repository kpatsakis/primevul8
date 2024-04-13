int mwifiex_set_secure_params(struct mwifiex_private *priv,
			      struct mwifiex_uap_bss_param *bss_config,
			      struct cfg80211_ap_settings *params) {
	int i;
	struct mwifiex_wep_key wep_key;

	if (!params->privacy) {
		bss_config->protocol = PROTOCOL_NO_SECURITY;
		bss_config->key_mgmt = KEY_MGMT_NONE;
		bss_config->wpa_cfg.length = 0;
		priv->sec_info.wep_enabled = 0;
		priv->sec_info.wpa_enabled = 0;
		priv->sec_info.wpa2_enabled = 0;

		return 0;
	}

	switch (params->auth_type) {
	case NL80211_AUTHTYPE_OPEN_SYSTEM:
		bss_config->auth_mode = WLAN_AUTH_OPEN;
		break;
	case NL80211_AUTHTYPE_SHARED_KEY:
		bss_config->auth_mode = WLAN_AUTH_SHARED_KEY;
		break;
	case NL80211_AUTHTYPE_NETWORK_EAP:
		bss_config->auth_mode = WLAN_AUTH_LEAP;
		break;
	default:
		bss_config->auth_mode = MWIFIEX_AUTH_MODE_AUTO;
		break;
	}

	bss_config->key_mgmt_operation |= KEY_MGMT_ON_HOST;

	for (i = 0; i < params->crypto.n_akm_suites; i++) {
		switch (params->crypto.akm_suites[i]) {
		case WLAN_AKM_SUITE_8021X:
			if (params->crypto.wpa_versions &
			    NL80211_WPA_VERSION_1) {
				bss_config->protocol = PROTOCOL_WPA;
				bss_config->key_mgmt = KEY_MGMT_EAP;
			}
			if (params->crypto.wpa_versions &
			    NL80211_WPA_VERSION_2) {
				bss_config->protocol |= PROTOCOL_WPA2;
				bss_config->key_mgmt = KEY_MGMT_EAP;
			}
			break;
		case WLAN_AKM_SUITE_PSK:
			if (params->crypto.wpa_versions &
			    NL80211_WPA_VERSION_1) {
				bss_config->protocol = PROTOCOL_WPA;
				bss_config->key_mgmt = KEY_MGMT_PSK;
			}
			if (params->crypto.wpa_versions &
			    NL80211_WPA_VERSION_2) {
				bss_config->protocol |= PROTOCOL_WPA2;
				bss_config->key_mgmt = KEY_MGMT_PSK;
			}
			break;
		default:
			break;
		}
	}
	for (i = 0; i < params->crypto.n_ciphers_pairwise; i++) {
		switch (params->crypto.ciphers_pairwise[i]) {
		case WLAN_CIPHER_SUITE_WEP40:
		case WLAN_CIPHER_SUITE_WEP104:
			break;
		case WLAN_CIPHER_SUITE_TKIP:
			if (params->crypto.wpa_versions & NL80211_WPA_VERSION_1)
				bss_config->wpa_cfg.pairwise_cipher_wpa |=
								CIPHER_TKIP;
			if (params->crypto.wpa_versions & NL80211_WPA_VERSION_2)
				bss_config->wpa_cfg.pairwise_cipher_wpa2 |=
								CIPHER_TKIP;
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			if (params->crypto.wpa_versions & NL80211_WPA_VERSION_1)
				bss_config->wpa_cfg.pairwise_cipher_wpa |=
								CIPHER_AES_CCMP;
			if (params->crypto.wpa_versions & NL80211_WPA_VERSION_2)
				bss_config->wpa_cfg.pairwise_cipher_wpa2 |=
								CIPHER_AES_CCMP;
		default:
			break;
		}
	}

	switch (params->crypto.cipher_group) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		if (priv->sec_info.wep_enabled) {
			bss_config->protocol = PROTOCOL_STATIC_WEP;
			bss_config->key_mgmt = KEY_MGMT_NONE;
			bss_config->wpa_cfg.length = 0;

			for (i = 0; i < NUM_WEP_KEYS; i++) {
				wep_key = priv->wep_key[i];
				bss_config->wep_cfg[i].key_index = i;

				if (priv->wep_key_curr_index == i)
					bss_config->wep_cfg[i].is_default = 1;
				else
					bss_config->wep_cfg[i].is_default = 0;

				bss_config->wep_cfg[i].length =
							     wep_key.key_length;
				memcpy(&bss_config->wep_cfg[i].key,
				       &wep_key.key_material,
				       wep_key.key_length);
			}
		}
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		bss_config->wpa_cfg.group_cipher = CIPHER_TKIP;
		break;
	case WLAN_CIPHER_SUITE_CCMP:
		bss_config->wpa_cfg.group_cipher = CIPHER_AES_CCMP;
		break;
	default:
		break;
	}

	return 0;
}