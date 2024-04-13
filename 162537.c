static int pb0100_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct gspca_dev *gspca_dev =
		container_of(ctrl->handler, struct gspca_dev, ctrl_handler);
	struct sd *sd = (struct sd *)gspca_dev;
	struct pb0100_ctrls *ctrls = sd->sensor_priv;
	int err = -EINVAL;

	switch (ctrl->id) {
	case V4L2_CID_AUTOGAIN:
		err = pb0100_set_autogain(gspca_dev, ctrl->val);
		if (err)
			break;
		if (ctrl->val)
			break;
		err = pb0100_set_gain(gspca_dev, ctrls->gain->val);
		if (err)
			break;
		err = pb0100_set_exposure(gspca_dev, ctrls->exposure->val);
		break;
	case V4L2_CTRL_CLASS_USER + 0x1001:
		err = pb0100_set_autogain_target(gspca_dev, ctrl->val);
		break;
	}
	return err;
}