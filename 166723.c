mlx5_fw_fatal_reporter_recover(struct devlink_health_reporter *reporter,
			       void *priv_ctx)
{
	struct mlx5_core_dev *dev = devlink_health_reporter_priv(reporter);

	return mlx5_health_try_recover(dev);
}