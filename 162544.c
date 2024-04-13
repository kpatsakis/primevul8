static int pb0100_init_controls(struct sd *sd)
{
	struct v4l2_ctrl_handler *hdl = &sd->gspca_dev.ctrl_handler;
	struct pb0100_ctrls *ctrls;
	static const struct v4l2_ctrl_config autogain_target = {
		.ops = &pb0100_ctrl_ops,
		.id = V4L2_CTRL_CLASS_USER + 0x1000,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Automatic Gain Target",
		.max = 255,
		.step = 1,
		.def = 128,
	};
	static const struct v4l2_ctrl_config natural_light = {
		.ops = &pb0100_ctrl_ops,
		.id = V4L2_CTRL_CLASS_USER + 0x1001,
		.type = V4L2_CTRL_TYPE_BOOLEAN,
		.name = "Natural Light Source",
		.max = 1,
		.step = 1,
		.def = 1,
	};

	ctrls = kzalloc(sizeof(*ctrls), GFP_KERNEL);
	if (!ctrls)
		return -ENOMEM;

	v4l2_ctrl_handler_init(hdl, 6);
	ctrls->autogain = v4l2_ctrl_new_std(hdl, &pb0100_ctrl_ops,
			V4L2_CID_AUTOGAIN, 0, 1, 1, 1);
	ctrls->exposure = v4l2_ctrl_new_std(hdl, &pb0100_ctrl_ops,
			V4L2_CID_EXPOSURE, 0, 511, 1, 12);
	ctrls->gain = v4l2_ctrl_new_std(hdl, &pb0100_ctrl_ops,
			V4L2_CID_GAIN, 0, 255, 1, 128);
	ctrls->red = v4l2_ctrl_new_std(hdl, &pb0100_ctrl_ops,
			V4L2_CID_RED_BALANCE, -255, 255, 1, 0);
	ctrls->blue = v4l2_ctrl_new_std(hdl, &pb0100_ctrl_ops,
			V4L2_CID_BLUE_BALANCE, -255, 255, 1, 0);
	ctrls->natural = v4l2_ctrl_new_custom(hdl, &natural_light, NULL);
	ctrls->target = v4l2_ctrl_new_custom(hdl, &autogain_target, NULL);
	if (hdl->error) {
		kfree(ctrls);
		return hdl->error;
	}
	sd->sensor_priv = ctrls;
	v4l2_ctrl_auto_cluster(5, &ctrls->autogain, 0, false);
	return 0;
}