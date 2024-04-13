proto_tree_add_node(proto_tree *tree, field_info *fi)
{
	proto_node *pnode, *tnode, *sibling;
	field_info *tfi;
	guint depth = 1;

	/*
	 * Restrict our depth. proto_tree_traverse_pre_order and
	 * proto_tree_traverse_post_order (and possibly others) are recursive
	 * so we need to be mindful of our stack size.
	 */
	if (tree->first_child == NULL) {
		for (tnode = tree; tnode != NULL; tnode = tnode->parent) {
			depth++;
			if (G_UNLIKELY(depth > prefs.gui_max_tree_depth)) {
				THROW_MESSAGE(DissectorError, wmem_strdup_printf(wmem_packet_scope(),
						     "Maximum tree depth %d exceeded for \"%s\" - \"%s\" (%s:%u) (Maximum depth can be increased in advanced preferences)",
						     prefs.gui_max_tree_depth,
						     fi->hfinfo->name, fi->hfinfo->abbrev, G_STRFUNC, __LINE__));
			}
		}
	}

	/*
	 * Make sure "tree" is ready to have subtrees under it, by
	 * checking whether it's been given an ett_ value.
	 *
	 * "PNODE_FINFO(tnode)" may be null; that's the case for the root
	 * node of the protocol tree.  That node is not displayed,
	 * so it doesn't need an ett_ value to remember whether it
	 * was expanded.
	 */
	tnode = tree;
	tfi = PNODE_FINFO(tnode);
	if (tfi != NULL && (tfi->tree_type < 0 || tfi->tree_type >= num_tree_types)) {
		REPORT_DISSECTOR_BUG("\"%s\" - \"%s\" tfi->tree_type: %d invalid (%s:%u)",
				     fi->hfinfo->name, fi->hfinfo->abbrev, tfi->tree_type, __FILE__, __LINE__);
		/* XXX - is it safe to continue here? */
	}

	pnode = wmem_new(PNODE_POOL(tree), proto_node);
	PROTO_NODE_INIT(pnode);
	pnode->parent = tnode;
	PNODE_FINFO(pnode) = fi;
	pnode->tree_data = PTREE_DATA(tree);

	if (tnode->last_child != NULL) {
		sibling = tnode->last_child;
		DISSECTOR_ASSERT(sibling->next == NULL);
		sibling->next = pnode;
	} else
		tnode->first_child = pnode;
	tnode->last_child = pnode;

	tree_data_add_maybe_interesting_field(pnode->tree_data, fi);

	return (proto_item *)pnode;
}