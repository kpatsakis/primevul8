static int pb0100_set_autogain_target(struct gspca_dev *gspca_dev, __s32 val)
{
	int err, totalpixels, brightpixels, darkpixels;
	struct sd *sd = (struct sd *) gspca_dev;

	/* Number of pixels counted by the sensor when subsampling the pixels.
	 * Slightly larger than the real value to avoid oscillation */
	totalpixels = gspca_dev->pixfmt.width * gspca_dev->pixfmt.height;
	totalpixels = totalpixels/(8*8) + totalpixels/(64*64);

	brightpixels = (totalpixels * val) >> 8;
	darkpixels   = totalpixels - brightpixels;
	err = stv06xx_write_sensor(sd, PB_R21, brightpixels);
	if (!err)
		err = stv06xx_write_sensor(sd, PB_R22, darkpixels);

	gspca_dbg(gspca_dev, D_CONF, "Set autogain target to %d, status: %d\n",
		  val, err);

	return err;
}