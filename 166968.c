proto_tree_set_bytes(field_info *fi, const guint8* start_ptr, gint length)
{
	GByteArray *bytes;

	DISSECTOR_ASSERT(start_ptr != NULL || length == 0);

	bytes = g_byte_array_new();
	if (length > 0) {
		g_byte_array_append(bytes, start_ptr, length);
	}
	fvalue_set_byte_array(&fi->value, bytes);
}