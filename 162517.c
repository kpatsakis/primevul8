static int pb0100_set_red_balance(struct gspca_dev *gspca_dev, __s32 val)
{
	int err;
	struct sd *sd = (struct sd *) gspca_dev;
	struct pb0100_ctrls *ctrls = sd->sensor_priv;

	val += ctrls->gain->val;
	if (val < 0)
		val = 0;
	else if (val > 255)
		val = 255;

	err = stv06xx_write_sensor(sd, PB_RGAIN, val);
	gspca_dbg(gspca_dev, D_CONF, "Set red gain to %d, status: %d\n",
		  val, err);

	return err;
}