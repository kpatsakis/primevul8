proto_tree_add_int(proto_tree *tree, int hfindex, tvbuff_t *tvb, gint start,
		   gint length, gint32 value)
{
	proto_item	  *pi = NULL;
	header_field_info *hfinfo;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	switch (hfinfo->type) {
		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
			pi = proto_tree_add_pi(tree, hfinfo, tvb, start, &length);
			proto_tree_set_int(PNODE_FINFO(pi), value);
			break;

		default:
			REPORT_DISSECTOR_BUG("field %s is not of type FT_INT8, FT_INT16, FT_INT24, or FT_INT32",
			    hfinfo->abbrev);
	}

	return pi;
}