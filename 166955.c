ptvcursor_add(ptvcursor_t *ptvc, int hfindex, gint length,
	      const guint encoding)
{
	field_info	  *new_fi;
	header_field_info *hfinfo;
	gint		   item_length;
	int		   offset;

	offset = ptvc->offset;
	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);
	get_hfi_length(hfinfo, ptvc->tvb, offset, &length, &item_length, encoding);
	test_length(hfinfo, ptvc->tvb, offset, item_length, encoding);

	ptvc->offset += get_full_length(hfinfo, ptvc->tvb, offset, length,
	    item_length, encoding);

	CHECK_FOR_NULL_TREE(ptvc->tree);

	/* Coast clear. Try and fake it */
	TRY_TO_FAKE_THIS_ITEM(ptvc->tree, hfindex, hfinfo);

	new_fi = new_field_info(ptvc->tree, hfinfo, ptvc->tvb, offset, item_length);

	return proto_tree_new_item(new_fi, ptvc->tree, ptvc->tvb,
		offset, length, encoding);
}