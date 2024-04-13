static int pb0100_set_exposure(struct gspca_dev *gspca_dev, __s32 val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	int err;

	err = stv06xx_write_sensor(sd, PB_RINTTIME, val);
	gspca_dbg(gspca_dev, D_CONF, "Set exposure to %d, status: %d\n",
		  val, err);

	return err;
}