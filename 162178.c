static void nl80211_finish_netdev_dump(struct cfg80211_registered_device *rdev)
{
	cfg80211_unlock_rdev(rdev);
	rtnl_unlock();
}