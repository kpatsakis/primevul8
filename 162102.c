static int nl80211_get_wowlan(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	struct sk_buff *msg;
	void *hdr;

	if (!rdev->wiphy.wowlan.flags && !rdev->wiphy.wowlan.n_patterns)
		return -EOPNOTSUPP;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	hdr = nl80211hdr_put(msg, info->snd_portid, info->snd_seq, 0,
			     NL80211_CMD_GET_WOWLAN);
	if (!hdr)
		goto nla_put_failure;

	if (rdev->wowlan) {
		struct nlattr *nl_wowlan;

		nl_wowlan = nla_nest_start(msg, NL80211_ATTR_WOWLAN_TRIGGERS);
		if (!nl_wowlan)
			goto nla_put_failure;

		if ((rdev->wowlan->any &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_ANY)) ||
		    (rdev->wowlan->disconnect &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_DISCONNECT)) ||
		    (rdev->wowlan->magic_pkt &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_MAGIC_PKT)) ||
		    (rdev->wowlan->gtk_rekey_failure &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_GTK_REKEY_FAILURE)) ||
		    (rdev->wowlan->eap_identity_req &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_EAP_IDENT_REQUEST)) ||
		    (rdev->wowlan->four_way_handshake &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_4WAY_HANDSHAKE)) ||
		    (rdev->wowlan->rfkill_release &&
		     nla_put_flag(msg, NL80211_WOWLAN_TRIG_RFKILL_RELEASE)))
			goto nla_put_failure;
		if (rdev->wowlan->n_patterns) {
			struct nlattr *nl_pats, *nl_pat;
			int i, pat_len;

			nl_pats = nla_nest_start(msg,
					NL80211_WOWLAN_TRIG_PKT_PATTERN);
			if (!nl_pats)
				goto nla_put_failure;

			for (i = 0; i < rdev->wowlan->n_patterns; i++) {
				nl_pat = nla_nest_start(msg, i + 1);
				if (!nl_pat)
					goto nla_put_failure;
				pat_len = rdev->wowlan->patterns[i].pattern_len;
				if (nla_put(msg, NL80211_WOWLAN_PKTPAT_MASK,
					    DIV_ROUND_UP(pat_len, 8),
					    rdev->wowlan->patterns[i].mask) ||
				    nla_put(msg, NL80211_WOWLAN_PKTPAT_PATTERN,
					    pat_len,
					    rdev->wowlan->patterns[i].pattern))
					goto nla_put_failure;
				nla_nest_end(msg, nl_pat);
			}
			nla_nest_end(msg, nl_pats);
		}

		nla_nest_end(msg, nl_wowlan);
	}

	genlmsg_end(msg, hdr);
	return genlmsg_reply(msg, info);

nla_put_failure:
	nlmsg_free(msg);
	return -ENOBUFS;
}