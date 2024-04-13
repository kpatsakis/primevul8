static void dbg_dump(int line_count, const char *func_name, unsigned char *buf,
		     unsigned int len)
{
	static char name[255];

	sprintf(name, "hso[%d:%s]", line_count, func_name);
	print_hex_dump_bytes(name, DUMP_PREFIX_NONE, buf, len);
}