int stv06xx_read_bridge(struct sd *sd, u16 address, u8 *i2c_data)
{
	int err;
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	struct usb_device *udev = sd->gspca_dev.dev;
	__u8 *buf = sd->gspca_dev.usb_buf;

	err = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
			      0x04, 0xc0, address, 0, buf, 1,
			      STV06XX_URB_MSG_TIMEOUT);

	*i2c_data = buf[0];

	gspca_dbg(gspca_dev, D_CONF, "Reading 0x%x from address 0x%x, status %d\n",
		  *i2c_data, address, err);

	return (err < 0) ? err : 0;
}