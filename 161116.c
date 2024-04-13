set_num_733(unsigned char *p, uint32_t value)
{
	archive_le32enc(p, value);
	archive_be32enc(p+4, value);
}