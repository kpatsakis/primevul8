proto_tree_set_ipv6(field_info *fi, const guint8* value_ptr)
{
	DISSECTOR_ASSERT(value_ptr != NULL);
	fvalue_set_bytes(&fi->value, value_ptr);
}