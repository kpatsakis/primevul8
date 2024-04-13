static int stv06xx_config(struct gspca_dev *gspca_dev,
			  const struct usb_device_id *id)
{
	struct sd *sd = (struct sd *) gspca_dev;

	gspca_dbg(gspca_dev, D_PROBE, "Configuring camera\n");

	sd->bridge = id->driver_info;
	gspca_dev->sd_desc = &sd_desc;

	if (dump_bridge)
		stv06xx_dump_bridge(sd);

	sd->sensor = &stv06xx_sensor_st6422;
	if (!sd->sensor->probe(sd))
		return 0;

	sd->sensor = &stv06xx_sensor_vv6410;
	if (!sd->sensor->probe(sd))
		return 0;

	sd->sensor = &stv06xx_sensor_hdcs1x00;
	if (!sd->sensor->probe(sd))
		return 0;

	sd->sensor = &stv06xx_sensor_hdcs1020;
	if (!sd->sensor->probe(sd))
		return 0;

	sd->sensor = &stv06xx_sensor_pb0100;
	if (!sd->sensor->probe(sd))
		return 0;

	sd->sensor = NULL;
	return -ENODEV;
}