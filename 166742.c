proto_tree_set_visible(proto_tree *tree, gboolean visible)
{
	gboolean old_visible = PTREE_DATA(tree)->visible;

	PTREE_DATA(tree)->visible = visible;

	return old_visible;
}