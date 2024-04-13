proto_tree_set_eui64(field_info *fi, const guint64 value)
{
	fvalue_set_uinteger64(&fi->value, value);
}