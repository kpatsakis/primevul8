proto_tracking_interesting_fields(const proto_tree *tree)
{
	GHashTable *interesting_hfids;

	if (!tree)
		return FALSE;

	interesting_hfids = PTREE_DATA(tree)->interesting_hfids;

	return (interesting_hfids != NULL) && g_hash_table_size(interesting_hfids);
}