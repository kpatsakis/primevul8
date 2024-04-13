static int stv06xx_init(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;
	int err;

	gspca_dbg(gspca_dev, D_PROBE, "Initializing camera\n");

	/* Let the usb init settle for a bit
	   before performing the initialization */
	msleep(250);

	err = sd->sensor->init(sd);

	if (dump_sensor && sd->sensor->dump)
		sd->sensor->dump(sd);

	return (err < 0) ? err : 0;
}