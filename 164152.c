void mwifiex_uap_set_channel(struct mwifiex_private *priv,
			     struct mwifiex_uap_bss_param *bss_cfg,
			     struct cfg80211_chan_def chandef)
{
	u8 config_bands = 0, old_bands = priv->adapter->config_bands;

	priv->bss_chandef = chandef;

	bss_cfg->channel = ieee80211_frequency_to_channel(
						     chandef.chan->center_freq);

	/* Set appropriate bands */
	if (chandef.chan->band == NL80211_BAND_2GHZ) {
		bss_cfg->band_cfg = BAND_CONFIG_BG;
		config_bands = BAND_B | BAND_G;

		if (chandef.width > NL80211_CHAN_WIDTH_20_NOHT)
			config_bands |= BAND_GN;
	} else {
		bss_cfg->band_cfg = BAND_CONFIG_A;
		config_bands = BAND_A;

		if (chandef.width > NL80211_CHAN_WIDTH_20_NOHT)
			config_bands |= BAND_AN;

		if (chandef.width > NL80211_CHAN_WIDTH_40)
			config_bands |= BAND_AAC;
	}

	switch (chandef.width) {
	case NL80211_CHAN_WIDTH_5:
	case NL80211_CHAN_WIDTH_10:
	case NL80211_CHAN_WIDTH_20_NOHT:
	case NL80211_CHAN_WIDTH_20:
		break;
	case NL80211_CHAN_WIDTH_40:
		if (chandef.center_freq1 < chandef.chan->center_freq)
			bss_cfg->band_cfg |= MWIFIEX_SEC_CHAN_BELOW;
		else
			bss_cfg->band_cfg |= MWIFIEX_SEC_CHAN_ABOVE;
		break;
	case NL80211_CHAN_WIDTH_80:
	case NL80211_CHAN_WIDTH_80P80:
	case NL80211_CHAN_WIDTH_160:
		bss_cfg->band_cfg |=
		    mwifiex_get_sec_chan_offset(bss_cfg->channel) << 4;
		break;
	default:
		mwifiex_dbg(priv->adapter,
			    WARN, "Unknown channel width: %d\n",
			    chandef.width);
		break;
	}

	priv->adapter->config_bands = config_bands;

	if (old_bands != config_bands) {
		mwifiex_send_domain_info_cmd_fw(priv->adapter->wiphy);
		mwifiex_dnld_txpwr_table(priv);
	}
}