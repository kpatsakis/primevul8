set_unused_field_bp(unsigned char *bp, int from, int to)
{
	memset(bp + from, 0, to - from + 1);
}