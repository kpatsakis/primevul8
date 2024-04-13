proto_get_finfo_ptr_array(const proto_tree *tree, const int id)
{
	if (!tree)
		return NULL;

	if (PTREE_DATA(tree)->interesting_hfids != NULL)
		return (GPtrArray *)g_hash_table_lookup(PTREE_DATA(tree)->interesting_hfids,
					   GINT_TO_POINTER(id));
	else
		return NULL;
}