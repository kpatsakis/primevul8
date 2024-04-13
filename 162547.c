static int sd_int_pkt_scan(struct gspca_dev *gspca_dev,
			u8 *data,		/* interrupt packet data */
			int len)		/* interrupt packet length */
{
	int ret = -EINVAL;

	if (len == 1 && (data[0] == 0x80 || data[0] == 0x10)) {
		input_report_key(gspca_dev->input_dev, KEY_CAMERA, 1);
		input_sync(gspca_dev->input_dev);
		ret = 0;
	}

	if (len == 1 && (data[0] == 0x88 || data[0] == 0x11)) {
		input_report_key(gspca_dev->input_dev, KEY_CAMERA, 0);
		input_sync(gspca_dev->input_dev);
		ret = 0;
	}

	return ret;
}