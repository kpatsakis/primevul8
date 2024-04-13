proto_tree_create_root(packet_info *pinfo)
{
	proto_node *pnode;

	/* Initialize the proto_node */
	pnode = g_slice_new(proto_tree);
	PROTO_NODE_INIT(pnode);
	pnode->parent = NULL;
	PNODE_FINFO(pnode) = NULL;
	pnode->tree_data = g_slice_new(tree_data_t);

	/* Make sure we can access pinfo everywhere */
	pnode->tree_data->pinfo = pinfo;

	/* Don't initialize the tree_data_t. Wait until we know we need it */
	pnode->tree_data->interesting_hfids = NULL;

	/* Set the default to FALSE so it's easier to
	 * find errors; if we expect to see the protocol tree
	 * but for some reason the default 'visible' is not
	 * changed, then we'll find out very quickly. */
	pnode->tree_data->visible = FALSE;

	/* Make sure that we fake protocols (if possible) */
	pnode->tree_data->fake_protocols = TRUE;

	/* Keep track of the number of children */
	pnode->tree_data->count = 0;

	return (proto_tree *)pnode;
}