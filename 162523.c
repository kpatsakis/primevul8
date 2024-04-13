static int pb0100_set_autogain(struct gspca_dev *gspca_dev, __s32 val)
{
	int err;
	struct sd *sd = (struct sd *) gspca_dev;
	struct pb0100_ctrls *ctrls = sd->sensor_priv;

	if (val) {
		if (ctrls->natural->val)
			val = BIT(6)|BIT(4)|BIT(0);
		else
			val = BIT(4)|BIT(0);
	} else
		val = 0;

	err = stv06xx_write_sensor(sd, PB_EXPGAIN, val);
	gspca_dbg(gspca_dev, D_CONF, "Set autogain to %d (natural: %d), status: %d\n",
		  val, ctrls->natural->val, err);

	return err;
}