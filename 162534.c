static int pb0100_stop(struct sd *sd)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	int err;

	err = stv06xx_write_sensor(sd, PB_ABORTFRAME, 1);

	if (err < 0)
		goto out;

	/* Set bit 1 to zero */
	err = stv06xx_write_sensor(sd, PB_CONTROL, BIT(5)|BIT(3));

	gspca_dbg(gspca_dev, D_STREAM, "Halting stream\n");
out:
	return (err < 0) ? err : 0;
}