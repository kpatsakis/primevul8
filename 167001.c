proto_tree_get_parent_tree(proto_tree *tree) {
	if (!tree)
		return NULL;

	/* we're the root tree, there's no parent
	   return ourselves so the caller has at least a tree to attach to */
	if (!tree->parent)
		return tree;

	return (proto_tree *)tree->parent;
}