static int pb0100_probe(struct sd *sd)
{
	u16 sensor;
	int err;

	err = stv06xx_read_sensor(sd, PB_IDENT, &sensor);

	if (err < 0)
		return -ENODEV;
	if ((sensor >> 8) != 0x64)
		return -ENODEV;

	pr_info("Photobit pb0100 sensor detected\n");

	sd->gspca_dev.cam.cam_mode = pb0100_mode;
	sd->gspca_dev.cam.nmodes = ARRAY_SIZE(pb0100_mode);

	return 0;
}