static void mlx5_fw_fatal_reporter_err_work(struct work_struct *work)
{
	struct mlx5_fw_reporter_ctx fw_reporter_ctx;
	struct mlx5_core_health *health;
	struct mlx5_core_dev *dev;
	struct mlx5_priv *priv;

	health = container_of(work, struct mlx5_core_health, fatal_report_work);
	priv = container_of(health, struct mlx5_priv, health);
	dev = container_of(priv, struct mlx5_core_dev, priv);

	mlx5_enter_error_state(dev, false);
	if (IS_ERR_OR_NULL(health->fw_fatal_reporter)) {
		if (mlx5_health_try_recover(dev))
			mlx5_core_err(dev, "health recovery failed\n");
		return;
	}
	fw_reporter_ctx.err_synd = health->synd;
	fw_reporter_ctx.miss_counter = health->miss_counter;
	devlink_health_report(health->fw_fatal_reporter,
			      "FW fatal error reported", &fw_reporter_ctx);
}