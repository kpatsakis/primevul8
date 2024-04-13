decode_bitfield_varint_value(char *buf, const guint64 val, const guint64 mask, const int width)
{
	char *p;

	p = other_decode_bitfield_varint_value(buf, val, mask, width);
	p = g_stpcpy(p, " = ");

	return p;
}