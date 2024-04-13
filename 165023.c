void _af_printid (uint32_t id)
{
	printf("%c%c%c%c",
		(id >> 24) & 0xff,
		(id >> 16) & 0xff,
		(id >> 8) & 0xff,
		id & 0xff);
}