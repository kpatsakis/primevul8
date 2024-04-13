mlx5_fw_reporter_dump(struct devlink_health_reporter *reporter,
		      struct devlink_fmsg *fmsg, void *priv_ctx)
{
	struct mlx5_core_dev *dev = devlink_health_reporter_priv(reporter);
	int err;

	err = mlx5_fw_tracer_trigger_core_dump_general(dev);
	if (err)
		return err;

	if (priv_ctx) {
		struct mlx5_fw_reporter_ctx *fw_reporter_ctx = priv_ctx;

		err = mlx5_fw_reporter_ctx_pairs_put(fmsg, fw_reporter_ctx);
		if (err)
			return err;
	}

	err = mlx5_fw_reporter_heath_buffer_data_put(dev, fmsg);
	if (err)
		return err;
	return mlx5_fw_tracer_get_saved_traces_objects(dev->tracer, fmsg);
}