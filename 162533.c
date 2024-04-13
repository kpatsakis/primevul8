static void sd_disconnect(struct usb_interface *intf)
{
	struct gspca_dev *gspca_dev = usb_get_intfdata(intf);
	struct sd *sd = (struct sd *) gspca_dev;
	void *priv = sd->sensor_priv;
	gspca_dbg(gspca_dev, D_PROBE, "Disconnecting the stv06xx device\n");

	sd->sensor = NULL;
	gspca_disconnect(intf);
	kfree(priv);
}