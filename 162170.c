static u32 rateset_to_mask(struct ieee80211_supported_band *sband,
			   u8 *rates, u8 rates_len)
{
	u8 i;
	u32 mask = 0;

	for (i = 0; i < rates_len; i++) {
		int rate = (rates[i] & 0x7f) * 5;
		int ridx;
		for (ridx = 0; ridx < sband->n_bitrates; ridx++) {
			struct ieee80211_rate *srate =
				&sband->bitrates[ridx];
			if (rate == srate->bitrate) {
				mask |= 1 << ridx;
				break;
			}
		}
		if (ridx == sband->n_bitrates)
			return 0; /* rate not found */
	}

	return mask;
}