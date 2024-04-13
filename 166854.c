proto_tree_set_protocol_tvb(field_info *fi, tvbuff_t *tvb, const char* field_data)
{
	fvalue_set_protocol(&fi->value, tvb, field_data);
}