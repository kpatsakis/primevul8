proto_tree_get_root(proto_tree *tree) {
	if (!tree)
		return NULL;
	while (tree->parent) {
		tree = tree->parent;
	}
	return tree;
}