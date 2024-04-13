proto_tree_children_foreach(proto_tree *tree, proto_tree_foreach_func func,
			    gpointer data)
{
	proto_node *node = tree;
	proto_node *current;

	if (!node)
		return;

	node = node->first_child;
	while (node != NULL) {
		current = node;
		node    = current->next;
		func((proto_tree *)current, data);
	}
}