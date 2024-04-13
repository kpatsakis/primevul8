proto_tree_add_subtree(proto_tree *tree, tvbuff_t *tvb, gint start, gint length, gint idx, proto_item **tree_item, const char *text)
{
	return proto_tree_add_subtree_format(tree, tvb, start, length, idx, tree_item, "%s", text);
}