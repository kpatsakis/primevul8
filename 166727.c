static void mlx5_fw_reporter_err_work(struct work_struct *work)
{
	struct mlx5_fw_reporter_ctx fw_reporter_ctx;
	struct mlx5_core_health *health;

	health = container_of(work, struct mlx5_core_health, report_work);

	if (IS_ERR_OR_NULL(health->fw_reporter))
		return;

	fw_reporter_ctx.err_synd = health->synd;
	fw_reporter_ctx.miss_counter = health->miss_counter;
	if (fw_reporter_ctx.err_synd) {
		devlink_health_report(health->fw_reporter,
				      "FW syndrom reported", &fw_reporter_ctx);
		return;
	}
	if (fw_reporter_ctx.miss_counter)
		devlink_health_report(health->fw_reporter,
				      "FW miss counter reported",
				      &fw_reporter_ctx);
}