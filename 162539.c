static int stv06xx_init_controls(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	gspca_dbg(gspca_dev, D_PROBE, "Initializing controls\n");

	gspca_dev->vdev.ctrl_handler = &gspca_dev->ctrl_handler;
	return sd->sensor->init_controls(sd);
}