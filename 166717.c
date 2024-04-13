mlx5_fw_reporter_ctx_pairs_put(struct devlink_fmsg *fmsg,
			       struct mlx5_fw_reporter_ctx *fw_reporter_ctx)
{
	int err;

	err = devlink_fmsg_u8_pair_put(fmsg, "syndrome",
				       fw_reporter_ctx->err_synd);
	if (err)
		return err;
	err = devlink_fmsg_u32_pair_put(fmsg, "fw_miss_counter",
					fw_reporter_ctx->miss_counter);
	if (err)
		return err;
	return 0;
}