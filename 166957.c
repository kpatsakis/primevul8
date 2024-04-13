proto_all_finfos(proto_tree *tree)
{
	ffdata_t ffdata;

	/* Pre allocate enough space to hold all fields in most cases */
	ffdata.array = g_ptr_array_sized_new(512);
	ffdata.id = 0;

	proto_tree_traverse_pre_order(tree, every_finfo, &ffdata);

	return ffdata.array;
}