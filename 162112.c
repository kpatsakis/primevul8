int nl80211_init(void)
{
	int err;

	err = genl_register_family_with_ops(&nl80211_fam,
		nl80211_ops, ARRAY_SIZE(nl80211_ops));
	if (err)
		return err;

	err = genl_register_mc_group(&nl80211_fam, &nl80211_config_mcgrp);
	if (err)
		goto err_out;

	err = genl_register_mc_group(&nl80211_fam, &nl80211_scan_mcgrp);
	if (err)
		goto err_out;

	err = genl_register_mc_group(&nl80211_fam, &nl80211_regulatory_mcgrp);
	if (err)
		goto err_out;

	err = genl_register_mc_group(&nl80211_fam, &nl80211_mlme_mcgrp);
	if (err)
		goto err_out;

#ifdef CONFIG_NL80211_TESTMODE
	err = genl_register_mc_group(&nl80211_fam, &nl80211_testmode_mcgrp);
	if (err)
		goto err_out;
#endif

	err = netlink_register_notifier(&nl80211_netlink_notifier);
	if (err)
		goto err_out;

	return 0;
 err_out:
	genl_unregister_family(&nl80211_fam);
	return err;
}