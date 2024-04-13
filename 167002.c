proto_tree_set_ether(field_info *fi, const guint8* value)
{
	fvalue_set_bytes(&fi->value, value);
}