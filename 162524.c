static int stv06xx_write_sensor_finish(struct sd *sd)
{
	int err = 0;

	if (sd->bridge == BRIDGE_STV610) {
		struct usb_device *udev = sd->gspca_dev.dev;
		__u8 *buf = sd->gspca_dev.usb_buf;

		buf[0] = 0;
		err = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
				      0x04, 0x40, 0x1704, 0, buf, 1,
				      STV06XX_URB_MSG_TIMEOUT);
	}

	return (err < 0) ? err : 0;
}