int stv06xx_write_bridge(struct sd *sd, u16 address, u16 i2c_data)
{
	int err;
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	struct usb_device *udev = sd->gspca_dev.dev;
	__u8 *buf = sd->gspca_dev.usb_buf;

	u8 len = (i2c_data > 0xff) ? 2 : 1;

	buf[0] = i2c_data & 0xff;
	buf[1] = (i2c_data >> 8) & 0xff;

	err = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			      0x04, 0x40, address, 0, buf, len,
			      STV06XX_URB_MSG_TIMEOUT);

	gspca_dbg(gspca_dev, D_CONF, "Written 0x%x to address 0x%x, status: %d\n",
		  i2c_data, address, err);

	return (err < 0) ? err : 0;
}