proto_tree_set_time(field_info *fi, const nstime_t *value_ptr)
{
	DISSECTOR_ASSERT(value_ptr != NULL);

	fvalue_set_time(&fi->value, value_ptr);
}