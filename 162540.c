static int pb0100_set_gain(struct gspca_dev *gspca_dev, __s32 val)
{
	int err;
	struct sd *sd = (struct sd *) gspca_dev;
	struct pb0100_ctrls *ctrls = sd->sensor_priv;

	err = stv06xx_write_sensor(sd, PB_G1GAIN, val);
	if (!err)
		err = stv06xx_write_sensor(sd, PB_G2GAIN, val);
	gspca_dbg(gspca_dev, D_CONF, "Set green gain to %d, status: %d\n",
		  val, err);

	if (!err)
		err = pb0100_set_red_balance(gspca_dev, ctrls->red->val);
	if (!err)
		err = pb0100_set_blue_balance(gspca_dev, ctrls->blue->val);

	return err;
}