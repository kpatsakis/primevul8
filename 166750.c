proto_tree_set_bytes_gbytearray(field_info *fi, const GByteArray *value)
{
	GByteArray *bytes;

	DISSECTOR_ASSERT(value != NULL);

	bytes = byte_array_dup(value);

	fvalue_set_byte_array(&fi->value, bytes);
}