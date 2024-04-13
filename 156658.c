static void copy_option(uint8_t *buf, uint16_t code, uint16_t len,
			uint8_t *msg)
{
	buf[0] = code >> 8;
	buf[1] = code & 0xff;
	buf[2] = len >> 8;
	buf[3] = len & 0xff;
	if (len > 0 && msg)
		memcpy(&buf[4], msg, len);
}