int stv06xx_write_sensor_bytes(struct sd *sd, const u8 *data, u8 len)
{
	int err, i, j;
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	struct usb_device *udev = sd->gspca_dev.dev;
	__u8 *buf = sd->gspca_dev.usb_buf;

	gspca_dbg(gspca_dev, D_CONF, "I2C: Command buffer contains %d entries\n",
		  len);
	for (i = 0; i < len;) {
		/* Build the command buffer */
		memset(buf, 0, I2C_BUFFER_LENGTH);
		for (j = 0; j < I2C_MAX_BYTES && i < len; j++, i++) {
			buf[j] = data[2*i];
			buf[0x10 + j] = data[2*i+1];
			gspca_dbg(gspca_dev, D_CONF, "I2C: Writing 0x%02x to reg 0x%02x\n",
				  data[2*i+1], data[2*i]);
		}
		buf[0x20] = sd->sensor->i2c_addr;
		buf[0x21] = j - 1; /* Number of commands to send - 1 */
		buf[0x22] = I2C_WRITE_CMD;
		err = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
				      0x04, 0x40, 0x0400, 0, buf,
				      I2C_BUFFER_LENGTH,
				      STV06XX_URB_MSG_TIMEOUT);
		if (err < 0)
			return err;
	}
	return stv06xx_write_sensor_finish(sd);
}