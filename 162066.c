static int nl80211_trigger_scan(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct wireless_dev *wdev = info->user_ptr[1];
	struct cfg80211_scan_request *request;
	struct nlattr *attr;
	struct wiphy *wiphy;
	int err, tmp, n_ssids = 0, n_channels, i;
	size_t ie_len;

	if (!is_valid_ie_attr(info->attrs[NL80211_ATTR_IE]))
		return -EINVAL;

	wiphy = &rdev->wiphy;

	if (!rdev->ops->scan)
		return -EOPNOTSUPP;

	if (rdev->scan_req)
		return -EBUSY;

	if (info->attrs[NL80211_ATTR_SCAN_FREQUENCIES]) {
		n_channels = validate_scan_freqs(
				info->attrs[NL80211_ATTR_SCAN_FREQUENCIES]);
		if (!n_channels)
			return -EINVAL;
	} else {
		enum ieee80211_band band;
		n_channels = 0;

		for (band = 0; band < IEEE80211_NUM_BANDS; band++)
			if (wiphy->bands[band])
				n_channels += wiphy->bands[band]->n_channels;
	}

	if (info->attrs[NL80211_ATTR_SCAN_SSIDS])
		nla_for_each_nested(attr, info->attrs[NL80211_ATTR_SCAN_SSIDS], tmp)
			n_ssids++;

	if (n_ssids > wiphy->max_scan_ssids)
		return -EINVAL;

	if (info->attrs[NL80211_ATTR_IE])
		ie_len = nla_len(info->attrs[NL80211_ATTR_IE]);
	else
		ie_len = 0;

	if (ie_len > wiphy->max_scan_ie_len)
		return -EINVAL;

	request = kzalloc(sizeof(*request)
			+ sizeof(*request->ssids) * n_ssids
			+ sizeof(*request->channels) * n_channels
			+ ie_len, GFP_KERNEL);
	if (!request)
		return -ENOMEM;

	if (n_ssids)
		request->ssids = (void *)&request->channels[n_channels];
	request->n_ssids = n_ssids;
	if (ie_len) {
		if (request->ssids)
			request->ie = (void *)(request->ssids + n_ssids);
		else
			request->ie = (void *)(request->channels + n_channels);
	}

	i = 0;
	if (info->attrs[NL80211_ATTR_SCAN_FREQUENCIES]) {
		/* user specified, bail out if channel not found */
		nla_for_each_nested(attr, info->attrs[NL80211_ATTR_SCAN_FREQUENCIES], tmp) {
			struct ieee80211_channel *chan;

			chan = ieee80211_get_channel(wiphy, nla_get_u32(attr));

			if (!chan) {
				err = -EINVAL;
				goto out_free;
			}

			/* ignore disabled channels */
			if (chan->flags & IEEE80211_CHAN_DISABLED)
				continue;

			request->channels[i] = chan;
			i++;
		}
	} else {
		enum ieee80211_band band;

		/* all channels */
		for (band = 0; band < IEEE80211_NUM_BANDS; band++) {
			int j;
			if (!wiphy->bands[band])
				continue;
			for (j = 0; j < wiphy->bands[band]->n_channels; j++) {
				struct ieee80211_channel *chan;

				chan = &wiphy->bands[band]->channels[j];

				if (chan->flags & IEEE80211_CHAN_DISABLED)
					continue;

				request->channels[i] = chan;
				i++;
			}
		}
	}

	if (!i) {
		err = -EINVAL;
		goto out_free;
	}

	request->n_channels = i;

	i = 0;
	if (info->attrs[NL80211_ATTR_SCAN_SSIDS]) {
		nla_for_each_nested(attr, info->attrs[NL80211_ATTR_SCAN_SSIDS], tmp) {
			if (nla_len(attr) > IEEE80211_MAX_SSID_LEN) {
				err = -EINVAL;
				goto out_free;
			}
			request->ssids[i].ssid_len = nla_len(attr);
			memcpy(request->ssids[i].ssid, nla_data(attr), nla_len(attr));
			i++;
		}
	}

	if (info->attrs[NL80211_ATTR_IE]) {
		request->ie_len = nla_len(info->attrs[NL80211_ATTR_IE]);
		memcpy((void *)request->ie,
		       nla_data(info->attrs[NL80211_ATTR_IE]),
		       request->ie_len);
	}

	for (i = 0; i < IEEE80211_NUM_BANDS; i++)
		if (wiphy->bands[i])
			request->rates[i] =
				(1 << wiphy->bands[i]->n_bitrates) - 1;

	if (info->attrs[NL80211_ATTR_SCAN_SUPP_RATES]) {
		nla_for_each_nested(attr,
				    info->attrs[NL80211_ATTR_SCAN_SUPP_RATES],
				    tmp) {
			enum ieee80211_band band = nla_type(attr);

			if (band < 0 || band >= IEEE80211_NUM_BANDS) {
				err = -EINVAL;
				goto out_free;
			}
			err = ieee80211_get_ratemask(wiphy->bands[band],
						     nla_data(attr),
						     nla_len(attr),
						     &request->rates[band]);
			if (err)
				goto out_free;
		}
	}

	if (info->attrs[NL80211_ATTR_SCAN_FLAGS]) {
		request->flags = nla_get_u32(
			info->attrs[NL80211_ATTR_SCAN_FLAGS]);
		if (((request->flags & NL80211_SCAN_FLAG_LOW_PRIORITY) &&
		     !(wiphy->features & NL80211_FEATURE_LOW_PRIORITY_SCAN)) ||
		    ((request->flags & NL80211_SCAN_FLAG_FLUSH) &&
		     !(wiphy->features & NL80211_FEATURE_SCAN_FLUSH))) {
			err = -EOPNOTSUPP;
			goto out_free;
		}
	}

	request->no_cck =
		nla_get_flag(info->attrs[NL80211_ATTR_TX_NO_CCK_RATE]);

	request->wdev = wdev;
	request->wiphy = &rdev->wiphy;
	request->scan_start = jiffies;

	rdev->scan_req = request;
	err = rdev_scan(rdev, request);

	if (!err) {
		nl80211_send_scan_start(rdev, wdev);
		if (wdev->netdev)
			dev_hold(wdev->netdev);
	} else {
 out_free:
		rdev->scan_req = NULL;
		kfree(request);
	}

	return err;
}