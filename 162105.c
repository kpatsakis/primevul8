static bool ht_rateset_to_mask(struct ieee80211_supported_band *sband,
			       u8 *rates, u8 rates_len,
			       u8 mcs[IEEE80211_HT_MCS_MASK_LEN])
{
	u8 i;

	memset(mcs, 0, IEEE80211_HT_MCS_MASK_LEN);

	for (i = 0; i < rates_len; i++) {
		int ridx, rbit;

		ridx = rates[i] / 8;
		rbit = BIT(rates[i] % 8);

		/* check validity */
		if ((ridx < 0) || (ridx >= IEEE80211_HT_MCS_MASK_LEN))
			return false;

		/* check availability */
		if (sband->ht_cap.mcs.rx_mask[ridx] & rbit)
			mcs[ridx] |= rbit;
		else
			return false;
	}

	return true;
}