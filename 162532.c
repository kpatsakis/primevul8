int stv06xx_read_sensor(struct sd *sd, const u8 address, u16 *value)
{
	int err;
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	struct usb_device *udev = sd->gspca_dev.dev;
	__u8 *buf = sd->gspca_dev.usb_buf;

	err = stv06xx_write_bridge(sd, STV_I2C_FLUSH, sd->sensor->i2c_flush);
	if (err < 0)
		return err;

	/* Clear mem */
	memset(buf, 0, I2C_BUFFER_LENGTH);

	buf[0] = address;
	buf[0x20] = sd->sensor->i2c_addr;
	buf[0x21] = 0;

	/* Read I2C register */
	buf[0x22] = I2C_READ_CMD;

	err = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			      0x04, 0x40, 0x1400, 0, buf, I2C_BUFFER_LENGTH,
			      STV06XX_URB_MSG_TIMEOUT);
	if (err < 0) {
		pr_err("I2C: Read error writing address: %d\n", err);
		return err;
	}

	err = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
			      0x04, 0xc0, 0x1410, 0, buf, sd->sensor->i2c_len,
			      STV06XX_URB_MSG_TIMEOUT);
	if (sd->sensor->i2c_len == 2)
		*value = buf[0] | (buf[1] << 8);
	else
		*value = buf[0];

	gspca_dbg(gspca_dev, D_CONF, "I2C: Read 0x%x from address 0x%x, status: %d\n",
		  *value, address, err);

	return (err < 0) ? err : 0;
}