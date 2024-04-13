inet_cidrtomask(uint8_t cidr)
{
	uint32_t mask = 0;
	int b;

	for (b = 0; b < cidr; b++)
		mask |= (1 << (31 - b));
	return ntohl(mask);
}