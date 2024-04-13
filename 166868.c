proto_tree_add_item_ret_time_string(proto_tree *tree, int hfindex,
	tvbuff_t *tvb,
	const gint start, gint length, const guint encoding,
	wmem_allocator_t *scope, char **retval)
{
	header_field_info *hfinfo = proto_registrar_get_nth(hfindex);
	field_info	  *new_fi;
	nstime_t    time_stamp;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	switch (hfinfo->type) {
	case FT_ABSOLUTE_TIME:
		get_time_value(tree, tvb, start, length, encoding, &time_stamp, FALSE);
		*retval = abs_time_to_str(scope, &time_stamp, (absolute_time_display_e)hfinfo->display, TRUE);
		break;
	case FT_RELATIVE_TIME:
		get_time_value(tree, tvb, start, length, encoding, &time_stamp, TRUE);
		*retval = rel_time_to_secs_str(scope, &time_stamp);
		break;
	default:
		REPORT_DISSECTOR_BUG("field %s is not of type FT_ABSOLUTE_TIME or FT_RELATIVE_TIME",
			hfinfo->abbrev);
	}

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(tree, hfinfo, tvb, start, length);

	switch (hfinfo->type) {

	case FT_ABSOLUTE_TIME:
	case FT_RELATIVE_TIME:
		proto_tree_set_time(new_fi, &time_stamp);
		break;
	default:
		g_assert_not_reached();
	}

	new_fi->flags |= (encoding & ENC_LITTLE_ENDIAN) ? FI_LITTLE_ENDIAN : FI_BIG_ENDIAN;

	return proto_tree_add_node(tree, new_fi);
}