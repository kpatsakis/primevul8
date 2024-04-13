set_num_721(unsigned char *p, uint16_t value)
{
	archive_le16enc(p, value);
}