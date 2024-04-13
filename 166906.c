proto_check_for_protocol_or_field(const proto_tree* tree, const int id)
{
	GPtrArray *ptrs = proto_get_finfo_ptr_array(tree, id);

	if (g_ptr_array_len(ptrs) > 0) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}