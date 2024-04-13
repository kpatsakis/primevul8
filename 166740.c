proto_tree_traverse_pre_order(proto_tree *tree, proto_tree_traverse_func func,
			      gpointer data)
{
	proto_node *pnode = tree;
	proto_node *child;
	proto_node *current;

	if (func(pnode, data))
		return TRUE;

	child = pnode->first_child;
	while (child != NULL) {
		/*
		 * The routine we call might modify the child, e.g. by
		 * freeing it, so we get the child's successor before
		 * calling that routine.
		 */
		current = child;
		child   = current->next;
		if (proto_tree_traverse_pre_order((proto_tree *)current, func, data))
			return TRUE;
	}

	return FALSE;
}