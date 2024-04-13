find_first_finfo(proto_node *node, gpointer data)
{
	field_info *fi = PNODE_FINFO(node);
	if (fi && fi->hfinfo) {
		if (fi->hfinfo->id == ((ffdata_t*)data)->id) {
			g_ptr_array_add(((ffdata_t*)data)->array, fi);

			/* Stop traversing. */
			return TRUE;
		}
	}

	/* Continue traversing. */
	return FALSE;
}