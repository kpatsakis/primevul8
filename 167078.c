proto_tree_free_node(proto_node *node, gpointer data _U_)
{
	field_info *finfo  = PNODE_FINFO(node);

	proto_tree_children_foreach(node, proto_tree_free_node, NULL);

	FVALUE_CLEANUP(&finfo->value);
}