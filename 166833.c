proto_find_finfo(proto_tree *tree, const int id)
{
	ffdata_t ffdata;

	ffdata.array = g_ptr_array_new();
	ffdata.id = id;

	proto_tree_traverse_pre_order(tree, find_finfo, &ffdata);

	return ffdata.array;
}