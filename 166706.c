static void mlx5_fw_reporters_destroy(struct mlx5_core_dev *dev)
{
	struct mlx5_core_health *health = &dev->priv.health;

	if (!IS_ERR_OR_NULL(health->fw_reporter))
		devlink_health_reporter_destroy(health->fw_reporter);

	if (!IS_ERR_OR_NULL(health->fw_fatal_reporter))
		devlink_health_reporter_destroy(health->fw_fatal_reporter);
}