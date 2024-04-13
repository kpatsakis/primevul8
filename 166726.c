mlx5_fw_reporter_heath_buffer_data_put(struct mlx5_core_dev *dev,
				       struct devlink_fmsg *fmsg)
{
	struct mlx5_core_health *health = &dev->priv.health;
	struct health_buffer __iomem *h = health->health;
	int err;
	int i;

	if (!ioread8(&h->synd))
		return 0;

	err = devlink_fmsg_pair_nest_start(fmsg, "health buffer");
	if (err)
		return err;
	err = devlink_fmsg_obj_nest_start(fmsg);
	if (err)
		return err;
	err = devlink_fmsg_arr_pair_nest_start(fmsg, "assert_var");
	if (err)
		return err;

	for (i = 0; i < ARRAY_SIZE(h->assert_var); i++) {
		err = devlink_fmsg_u32_put(fmsg, ioread32be(h->assert_var + i));
		if (err)
			return err;
	}
	err = devlink_fmsg_arr_pair_nest_end(fmsg);
	if (err)
		return err;
	err = devlink_fmsg_u32_pair_put(fmsg, "assert_exit_ptr",
					ioread32be(&h->assert_exit_ptr));
	if (err)
		return err;
	err = devlink_fmsg_u32_pair_put(fmsg, "assert_callra",
					ioread32be(&h->assert_callra));
	if (err)
		return err;
	err = devlink_fmsg_u32_pair_put(fmsg, "hw_id", ioread32be(&h->hw_id));
	if (err)
		return err;
	err = devlink_fmsg_u8_pair_put(fmsg, "irisc_index",
				       ioread8(&h->irisc_index));
	if (err)
		return err;
	err = devlink_fmsg_u8_pair_put(fmsg, "synd", ioread8(&h->synd));
	if (err)
		return err;
	err = devlink_fmsg_u32_pair_put(fmsg, "ext_synd",
					ioread16be(&h->ext_synd));
	if (err)
		return err;
	err = devlink_fmsg_u32_pair_put(fmsg, "raw_fw_ver",
					ioread32be(&h->fw_ver));
	if (err)
		return err;
	err = devlink_fmsg_obj_nest_end(fmsg);
	if (err)
		return err;
	return devlink_fmsg_pair_nest_end(fmsg);
}