proto_tree_add_uint(proto_tree *tree, int hfindex, tvbuff_t *tvb, gint start,
		    gint length, guint32 value)
{
	proto_item	  *pi = NULL;
	header_field_info *hfinfo;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	switch (hfinfo->type) {
		case FT_CHAR:
		case FT_UINT8:
		case FT_UINT16:
		case FT_UINT24:
		case FT_UINT32:
		case FT_FRAMENUM:
			pi = proto_tree_add_pi(tree, hfinfo, tvb, start, &length);
			proto_tree_set_uint(PNODE_FINFO(pi), value);
			break;

		default:
			REPORT_DISSECTOR_BUG("field %s is not of type FT_CHAR, FT_UINT8, FT_UINT16, FT_UINT24, FT_UINT32, or FT_FRAMENUM",
			    hfinfo->abbrev);
	}

	return pi;
}