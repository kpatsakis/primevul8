proto_tree_set_fake_protocols(proto_tree *tree, gboolean fake_protocols)
{
	PTREE_DATA(tree)->fake_protocols = fake_protocols;
}