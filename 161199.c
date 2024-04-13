set_num_732(unsigned char *p, uint32_t value)
{
	archive_be32enc(p, value);
}