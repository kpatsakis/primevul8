proto_tree_add_item_new(proto_tree *tree, header_field_info *hfinfo, tvbuff_t *tvb,
			const gint start, gint length, const guint encoding)
{
	field_info        *new_fi;
	gint		  item_length;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	get_hfi_length(hfinfo, tvb, start, &length, &item_length, encoding);
	test_length(hfinfo, tvb, start, item_length, encoding);

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(tree, hfinfo, tvb, start, item_length);

	return proto_tree_new_item(new_fi, tree, tvb, start, length, encoding);
}