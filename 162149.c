static inline u64 wdev_id(struct wireless_dev *wdev)
{
	return (u64)wdev->identifier |
	       ((u64)wiphy_to_dev(wdev->wiphy)->wiphy_idx << 32);
}