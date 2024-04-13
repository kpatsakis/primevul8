static void stv06xx_stopN(struct gspca_dev *gspca_dev)
{
	int err;
	struct sd *sd = (struct sd *) gspca_dev;

	/* stop ISO-streaming */
	err = stv06xx_write_bridge(sd, STV_ISO_ENABLE, 0);
	if (err < 0)
		goto out;

	err = sd->sensor->stop(sd);

out:
	if (err < 0)
		gspca_dbg(gspca_dev, D_STREAM, "Failed to stop stream\n");
	else
		gspca_dbg(gspca_dev, D_STREAM, "Stopped streaming\n");
}