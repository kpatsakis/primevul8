set_num_723(unsigned char *p, uint16_t value)
{
	archive_le16enc(p, value);
	archive_be16enc(p+2, value);
}