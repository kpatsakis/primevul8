static int nl80211_netlink_notify(struct notifier_block * nb,
				  unsigned long state,
				  void *_notify)
{
	struct netlink_notify *notify = _notify;
	struct cfg80211_registered_device *rdev;
	struct wireless_dev *wdev;
	struct cfg80211_beacon_registration *reg, *tmp;

	if (state != NETLINK_URELEASE)
		return NOTIFY_DONE;

	rcu_read_lock();

	list_for_each_entry_rcu(rdev, &cfg80211_rdev_list, list) {
		list_for_each_entry_rcu(wdev, &rdev->wdev_list, list)
			cfg80211_mlme_unregister_socket(wdev, notify->portid);

		spin_lock_bh(&rdev->beacon_registrations_lock);
		list_for_each_entry_safe(reg, tmp, &rdev->beacon_registrations,
					 list) {
			if (reg->nlportid == notify->portid) {
				list_del(&reg->list);
				kfree(reg);
				break;
			}
		}
		spin_unlock_bh(&rdev->beacon_registrations_lock);
	}

	rcu_read_unlock();

	return NOTIFY_DONE;
}