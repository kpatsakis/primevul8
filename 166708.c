mlx5_fw_reporter_diagnose(struct devlink_health_reporter *reporter,
			  struct devlink_fmsg *fmsg)
{
	struct mlx5_core_dev *dev = devlink_health_reporter_priv(reporter);
	struct mlx5_core_health *health = &dev->priv.health;
	struct health_buffer __iomem *h = health->health;
	u8 synd;
	int err;

	synd = ioread8(&h->synd);
	err = devlink_fmsg_u8_pair_put(fmsg, "Syndrome", synd);
	if (err || !synd)
		return err;
	return devlink_fmsg_string_pair_put(fmsg, "Description", hsynd_str(synd));
}