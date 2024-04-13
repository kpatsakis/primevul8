check_for_undecoded(proto_node *node, gpointer data)
{
	field_info *fi = PNODE_FINFO(node);
	decoded_data_t* decoded = (decoded_data_t*)data;
	gint i;
	guint byte;
	guint bit;

	if (fi && fi->hfinfo->type != FT_PROTOCOL) {
		for (i = fi->start; i < fi->start + fi->length && i < decoded->length; i++) {
			byte = i / 8;
			bit = i % 8;
			decoded->buf[byte] |= (1 << bit);
		}
	}

	return FALSE;
}