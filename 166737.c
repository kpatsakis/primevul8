proto_tree_add_item_new_ret_length(proto_tree *tree, header_field_info *hfinfo,
				   tvbuff_t *tvb, const gint start,
				   gint length, const guint encoding,
				   gint *lenretval)
{
	field_info        *new_fi;
	gint		  item_length;
	proto_item	 *item;

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	get_hfi_length(hfinfo, tvb, start, &length, &item_length, encoding);
	test_length(hfinfo, tvb, start, item_length, encoding);

	if (!tree) {
		/*
		 * We need to get the correct item length here.
		 * That's normally done by proto_tree_new_item(),
		 * but we won't be calling it.
		 */
		*lenretval = get_full_length(hfinfo, tvb, start, length,
		    item_length, encoding);
		return NULL;
	}

	TRY_TO_FAKE_THIS_ITEM_OR_FREE(tree, hfinfo->id, hfinfo, {
		/*
		 * Even if the tree item is not referenced (and thus faked),
		 * the caller must still be informed of the actual length.
		 */
		*lenretval = get_full_length(hfinfo, tvb, start, length,
		    item_length, encoding);
	});

	new_fi = new_field_info(tree, hfinfo, tvb, start, item_length);

	item = proto_tree_new_item(new_fi, tree, tvb, start, length, encoding);
	*lenretval = new_fi->length;
	return item;
}