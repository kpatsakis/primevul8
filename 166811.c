proto_tree_add_int64(proto_tree *tree, int hfindex, tvbuff_t *tvb, gint start,
		     gint length, gint64 value)
{
	proto_item	  *pi = NULL;
	header_field_info *hfinfo;

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfindex, hfinfo);

	switch (hfinfo->type) {
		case FT_INT40:
		case FT_INT48:
		case FT_INT56:
		case FT_INT64:
			pi = proto_tree_add_pi(tree, hfinfo, tvb, start, &length);
			proto_tree_set_int64(PNODE_FINFO(pi), value);
			break;

		default:
			REPORT_DISSECTOR_BUG("field %s is not of type FT_INT40, FT_INT48, FT_INT56, or FT_INT64",
			    hfinfo->abbrev);
	}

	return pi;
}