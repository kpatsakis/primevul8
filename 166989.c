proto_item_get_subtree(proto_item *pi) {
	field_info *fi;

	if (!pi)
		return NULL;
	fi = PITEM_FINFO(pi);
	if ( (!fi) || (fi->tree_type == -1) )
		return NULL;
	return (proto_tree *)pi;
}