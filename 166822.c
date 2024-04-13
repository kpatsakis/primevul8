proto_tree_set_ipv4(field_info *fi, ws_in4_addr value)
{
	fvalue_set_uinteger(&fi->value, value);
}