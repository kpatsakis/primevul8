proto_tree_set_guid(field_info *fi, const e_guid_t *value_ptr)
{
	DISSECTOR_ASSERT(value_ptr != NULL);
	fvalue_set_guid(&fi->value, value_ptr);
}