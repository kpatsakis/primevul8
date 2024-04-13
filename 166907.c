proto_item_add_subtree(proto_item *pi,	const gint idx) {
	field_info *fi;

	if (!pi)
		return NULL;

	DISSECTOR_ASSERT(idx >= 0 && idx < num_tree_types);

	fi = PITEM_FINFO(pi);
	if (!fi)
		return (proto_tree *)pi;

	fi->tree_type = idx;

	return (proto_tree *)pi;
}