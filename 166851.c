proto_tree_get_parent(proto_tree *tree) {
	if (!tree)
		return NULL;
	return (proto_item *)tree;
}