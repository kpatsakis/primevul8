static void mlx5_fw_reporters_create(struct mlx5_core_dev *dev)
{
	struct mlx5_core_health *health = &dev->priv.health;
	struct devlink *devlink = priv_to_devlink(dev);

	health->fw_reporter =
		devlink_health_reporter_create(devlink, &mlx5_fw_reporter_ops,
					       0, false, dev);
	if (IS_ERR(health->fw_reporter))
		mlx5_core_warn(dev, "Failed to create fw reporter, err = %ld\n",
			       PTR_ERR(health->fw_reporter));

	health->fw_fatal_reporter =
		devlink_health_reporter_create(devlink,
					       &mlx5_fw_fatal_reporter_ops,
					       MLX5_REPORTER_FW_GRACEFUL_PERIOD,
					       true, dev);
	if (IS_ERR(health->fw_fatal_reporter))
		mlx5_core_warn(dev, "Failed to create fw fatal reporter, err = %ld\n",
			       PTR_ERR(health->fw_fatal_reporter));
}