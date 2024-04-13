proto_tree_add_text_node(proto_tree *tree, tvbuff_t *tvb, gint start, gint length)
{
	proto_item *pi;

	if (tree == NULL)
		return NULL;

	pi = proto_tree_add_pi(tree, &hfi_text_only, tvb, start, &length);

	return pi;
}