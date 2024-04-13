check_for_offset(proto_node *node, gpointer data)
{
	field_info	*fi        = PNODE_FINFO(node);
	offset_search_t	*offsearch = (offset_search_t *)data;

	/* !fi == the top most container node which holds nothing */
	if (fi && !proto_item_is_hidden(node) && !proto_item_is_generated(node) && fi->ds_tvb && offsearch->tvb == fi->ds_tvb) {
		if (offsearch->offset >= (guint) fi->start &&
				offsearch->offset < (guint) (fi->start + fi->length)) {

			offsearch->finfo = fi;
			return FALSE; /* keep traversing */
		}
	}
	return FALSE; /* keep traversing */
}